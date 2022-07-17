#include "usbip_setupdi.h"
#include "usbip_stub.h"
#include "usbip_util.h"
#include "usbip_common.h"

#include <stdlib.h>
#include <stdio.h>
#include <cfgmgr32.h>

char *get_dev_property(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data, DWORD prop);

BOOL build_cat(const char *path, const char *catname, const char *hwid);
int sign_file(LPCSTR subject, LPCSTR fpath);

/* should be same with strings in usbip_stub.inx */
#define STUB_DESC	"USB/IP STUB"
#define STUB_MFGNAME	"USB/IP Project"

static BOOL
get_drvinfo(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data, BOOL is_stub, PSP_DRVINFO_DATA pdrvinfo)
{
	int	i;

	pdrvinfo->cbSize = sizeof(SP_DRVINFO_DATA);

	if (!SetupDiBuildDriverInfoList(dev_info, pdev_info_data, SPDIT_COMPATDRIVER)) {
		dbg("failed to build driver info: 0x%lx", GetLastError());
		return FALSE;
	}

	for (i = 0;; i++) {
		if (!SetupDiEnumDriverInfoA(dev_info, pdev_info_data, SPDIT_COMPATDRIVER, i, pdrvinfo))
			break;
		if (is_stub) {
			if (strcmp(pdrvinfo->Description, STUB_DESC) == 0 && strcmp(pdrvinfo->MfgName, STUB_MFGNAME) == 0)
				return TRUE;
		}
		else
			return TRUE;
	}

	return FALSE;
}

static BOOL
exist_stub_driver(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data)
{
	SP_DRVINFO_DATA	drvinfo;
	return get_drvinfo(dev_info, pdev_info_data, TRUE, &drvinfo);
}

BOOL
is_service_usbip_stub(HDEVINFO dev_info, SP_DEVINFO_DATA *dev_info_data)
{
	char	*svcname;
	BOOL	res;

	svcname = get_dev_property(dev_info, dev_info_data, SPDRP_SERVICE);
	if (svcname == NULL)
		return FALSE;
	res = _stricmp(svcname, STUB_DRIVER_SVCNAME) == 0 ? TRUE: FALSE;
	free(svcname);
	return res;
}

static void
copy_file(const char *fname, const char *path_drvpkg)
{
	char	*path_src, *path_dst;

	path_src = get_abspath_from_modpath(fname);
	asprintf(&path_dst, "%s\\%s", path_drvpkg, fname);

	CopyFile(path_src, path_dst, TRUE);
	free(path_src);
	free(path_dst);
}

static void
translate_inf(const char *id_hw, FILE *in, FILE *out)
{
	char	buf[4096];
	char	*line = fgets(buf, 4096, in);

	while (line) {
		char	*mark;

		mark = strstr(line, "%hwid%");
		if (mark) {
			strcpy_s(mark, 4096 - (mark - buf), id_hw);
		}
		fwrite(line, strlen(line), 1, out);

		line = fgets(buf, 4096, in);
	}
}

static void
copy_stub_inf(const char *id_hw, const char *path_drvpkg)
{
	char	*path_inx, *path_dst;
	FILE	*in, *out;
	errno_t	err;

	path_inx = get_abspath_from_modpath("usbip_stub.inx");
	err = fopen_s(&in, path_inx, "r");
	free(path_inx);
	if (err != 0) {
		return;
	}
	asprintf(&path_dst, "%s\\usbip_stub.inf", path_drvpkg);
	err = fopen_s(&out, path_dst, "w");
	free(path_dst);
	if (err != 0) {
		fclose(in);
		return;
	}

	translate_inf(id_hw, in, out);
	fclose(in);
	fclose(out);
}

static void
remove_dir_all(const char *path_dir)
{
	char	*fpat;
	WIN32_FIND_DATA	wfd;
	HANDLE	hfs;

	asprintf(&fpat, "%s\\*", path_dir);
	hfs = FindFirstFile(fpat, &wfd);
	free(fpat);
	if (hfs != INVALID_HANDLE_VALUE) {
		do {
			if (*wfd.cFileName != '.') {
				char	*fpath;
				asprintf(&fpath, "%s\\%s", path_dir, wfd.cFileName);
				DeleteFile(fpath);
				free(fpath);
			}
		} while (FindNextFile(hfs, &wfd));

		FindClose(hfs);
	}
	RemoveDirectory(path_dir);
}

static BOOL
get_temp_drvpkg_path(char path_drvpkg[])
{
	char	tempdir[MAX_PATH + 1];

	if (GetTempPath(MAX_PATH + 1, tempdir) == 0)
		return FALSE;
	if (GetTempFileName(tempdir, "stub", 0, path_drvpkg) > 0) {
		DeleteFile(path_drvpkg);
		if (CreateDirectory(path_drvpkg, NULL))
			return TRUE;
	}
	else
		DeleteFile(path_drvpkg);
	return FALSE;
}

static int
install_stub_driver(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data)
{
	char	path_drvpkg[MAX_PATH + 1];
	char	*id_hw, *path_cat;
	char	*path_inf;
	int	ret;

	id_hw = get_id_hw(dev_info, pdev_info_data);
	if (id_hw == NULL)
		return ERR_GENERAL;
	if (!get_temp_drvpkg_path(path_drvpkg)) {
		free(id_hw);
		return ERR_GENERAL;
	}
	copy_file("usbip_stub.sys", path_drvpkg);
	copy_stub_inf(id_hw, path_drvpkg);

	if (!build_cat(path_drvpkg, "usbip_stub.cat", id_hw)) {
		remove_dir_all(path_drvpkg);
		free(id_hw);
		return ERR_GENERAL;
	}

	asprintf(&path_cat, "%s\\usbip_stub.cat", path_drvpkg);
	if ((ret = sign_file("USBIP Test", path_cat)) < 0) {
		remove_dir_all(path_drvpkg);
		free(path_cat);
		free(id_hw);
		if (ret == ERR_NOTEXIST)
			return ERR_CERTIFICATE;
		return ERR_GENERAL;
	}

	free(path_cat);

	/* install oem driver */
	asprintf(&path_inf, "%s\\usbip_stub.inf", path_drvpkg);
	if (!SetupCopyOEMInf(path_inf, NULL, SPOST_PATH, 0, NULL, 0, NULL, NULL)) {
		DWORD	err = GetLastError();
		dbg("failed to SetupCopyOEMInf: 0x%lx", err);
		ret = ERR_GENERAL;
	}
	free(path_inf);
	free(id_hw);

	remove_dir_all(path_drvpkg);
	return ret;
}

static int
try_to_install_stub_driver(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data)
{
	if (exist_stub_driver(dev_info, pdev_info_data))
		return 0;

	return install_stub_driver(dev_info, pdev_info_data);
}

static char *
get_stub_inf(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data, PSP_DRVINFO_DATA pdrvinfo)
{
	PSP_DRVINFO_DETAIL_DATA	pdrvinfo_detail;
	DWORD	reqsize;
	char	*buf, *inf_name;

	if (!SetupDiGetDriverInfoDetail(dev_info, pdev_info_data, pdrvinfo, NULL, 0, &reqsize))
		return NULL;
	buf = (char *)malloc(reqsize);
	if (buf == NULL)
		return NULL;
	pdrvinfo_detail = (PSP_DRVINFO_DETAIL_DATA)buf;
	pdrvinfo_detail->cbSize = sizeof(SP_DRVINFO_DETAIL_DATA);
	if (!SetupDiGetDriverInfoDetail(dev_info, pdev_info_data, pdrvinfo, pdrvinfo_detail, reqsize, &reqsize)) {
		free(buf);
		return NULL;
	}
	inf_name = _strdup(pdrvinfo_detail->InfFileName);
	if (pdrvinfo_detail->InfFileName) {
		char	*sep_last = strrchr(pdrvinfo_detail->InfFileName, '\\');
		if (sep_last)
			inf_name = _strdup(sep_last + 1);
		else
			inf_name = _strdup(pdrvinfo_detail->InfFileName);
	}
	free(buf);
	return inf_name;
}

static void
try_to_uninstall_stub_driver(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data)
{
	SP_DRVINFO_DATA	drvinfo;
	char	*inf_name;

	if (!get_drvinfo(dev_info, pdev_info_data, TRUE, &drvinfo))
		return;
	inf_name = get_stub_inf(dev_info, pdev_info_data, &drvinfo);
	if (inf_name) {
		if (!SetupUninstallOEMInf(inf_name, 0, NULL)) {
			dbg("failed to uninstall stub driver package: 0x%lx", GetLastError());
		}
		free(inf_name);
	}
}

static BOOL
is_reboot_required(DWORD devInst)
{
	ULONG	status, probno;

	if (CM_Get_DevNode_Status(&status, &probno, devInst, 0) != CR_SUCCESS) {
		dbg("failed to get devnode status: 0x%lx", GetLastError());
		return FALSE;
	}
	if (status & DN_NEED_RESTART)
		return TRUE;
	return FALSE;
}

static int
apply_driver(const char *inst_id, BOOL is_stub, PBOOL pneed_reboot)
{
	HDEVINFO	dev_info;
	SP_DEVINFO_DATA	dev_info_data;
	SP_DRVINFO_DATA	drvinfo;
	SP_DEVINSTALL_PARAMS	devinstparams;
	SP_DRVINSTALL_PARAMS	drvinstparams;
	int	ret = ERR_GENERAL;

	dev_info = SetupDiCreateDeviceInfoList(NULL, NULL);
	dev_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
	if (!SetupDiOpenDeviceInfoA(dev_info, inst_id, NULL, 0, &dev_info_data)) {
		dbg("failed to open device info: 0x%lx", GetLastError());
		goto out;
	}
	if (!get_drvinfo(dev_info, &dev_info_data, is_stub, &drvinfo)) {
		dbg("failed to get stub driver info: 0x%lx", GetLastError());
		goto out;
	}
	if (!SetupDiSetSelectedDriver(dev_info, &dev_info_data, &drvinfo)) {
		dbg("failed to select stub driver: 0x%lx", GetLastError());
		goto out;
	}

	drvinstparams.cbSize = sizeof(SP_DRVINSTALL_PARAMS);
	if (SetupDiGetDriverInstallParams(dev_info, &dev_info_data, &drvinfo, &drvinstparams)) {
		if (drvinstparams.Flags & DNF_INSTALLEDDRIVER) {
			ret = ERR_EXIST;
			goto out;
		}
	}
	devinstparams.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
	if (SetupDiGetDeviceInstallParams(dev_info, &dev_info_data, &devinstparams)) {
		devinstparams.Flags += DI_QUIETINSTALL;
		SetupDiSetDeviceInstallParams(dev_info, &dev_info_data, &devinstparams);
	}
	if (!SetupDiInstallDevice(dev_info, &dev_info_data)) {
		dbg("failed to install stub driver: 0x%lx", GetLastError());
		goto out;
	}
	if (pneed_reboot)
		*pneed_reboot = is_reboot_required(dev_info_data.DevInst);
	ret = 0;
out:
	SetupDiDestroyDeviceInfoList(dev_info);
	return ret;
}

static int
update_driver(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data)
{
	char	*inst_id;
	int	n_tries = 0;
	BOOL	need_reboot;
	int	ret;

	ret = try_to_install_stub_driver(dev_info, pdev_info_data);
	if (ret < 0)
		return ret;
	inst_id = get_id_inst(dev_info, pdev_info_data);
	while (TRUE) {
		ret = apply_driver(inst_id, TRUE, &need_reboot);
		if (ret != 0 || !need_reboot)
			break;
		/*
		 * Retry to install if reboot is required.
		 * This means that stub driver is not installed properly by other PNP activities.
		 */
		if (n_tries >= 5) {
			dbg("tried many times to install stub but failed");
			ret = ERR_BADBIND;
			break;
		}
		n_tries++;
		if (apply_driver(inst_id, FALSE, NULL) != 0) {
			dbg("failed to rollback for reinstall stub");
			ret = ERR_BADBIND;
			break;
		}
	}

	free(inst_id);
	if (ret == ERR_EXIST)
		return ERR_ALREADYBIND;
	return ret;
}

static int
rollback_driver(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data)
{
	char	*inst_id;
	int	ret;

	inst_id = get_id_inst(dev_info, pdev_info_data);
	ret = apply_driver(inst_id, FALSE, NULL);
	free(inst_id);
	if (ret == ERR_EXIST)
		return ERR_NOTBIND;
	if (ret == 0)
		try_to_uninstall_stub_driver(dev_info, pdev_info_data);
	return ret;
}

static int
walker_attach(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data, devno_t devno, void *ctx)
{
	devno_t	*pdevno = (devno_t *)ctx;

	if (devno == *pdevno) {
		int	ret;

		ret = update_driver(dev_info, pdev_info_data);
		if (ret < 0)
			return ret;
		return 1;
	}
	return 0;
}

int
attach_stub_driver(devno_t devno)
{
	int	ret;

	ret = traverse_usbdevs(walker_attach, TRUE, &devno);
	switch (ret) {
	case 0:
		return ERR_NOTEXIST;
	case 1:
		return 0;
	default:
		return ret;
	}
}

static int
walker_detach(HDEVINFO dev_info, PSP_DEVINFO_DATA pdev_info_data, devno_t devno, void *ctx)
{
	devno_t	*pdevno = (devno_t *)ctx;

	if (devno == *pdevno) {
		int	ret;

		ret = rollback_driver(dev_info, pdev_info_data);
		if (ret < 0)
			return ret;
		return 1;
	}
	return 0;
}

int
detach_stub_driver(devno_t devno)
{
	int	ret;

	ret = traverse_usbdevs(walker_detach, TRUE, &devno);
	switch (ret) {
	case 0:
		return ERR_NOTEXIST;
	case 1:
		return 0;
	default:
		return ret;
	}
}
