:: Copy driver files into output folder.
:: All binary files will be stored in $(OutDir)\package\usbip_win
:: $(OutDir)\$(ProjectName)
cd %1\%2
:: mkdir $(SolutionName)
md %3
cd %3
copy /y ..\..\attacher.exe
copy /y ..\..\..\..\userspace\usb.ids
copy /y usb.ids ..\..\
copy /y ..\..\usbip.exe
copy /y ..\..\usbipd.exe
copy /y ..\..\usbip_root.inf
copy /y ..\..\..\..\driver\stub\usbip_stub.inx
copy /y usbip_stub.inx ..\..\
copy /y ..\..\usbip_stub.sys
copy /y ..\..\..\..\driver\usbip_test.pfx
copy /y usbip_test.pfx ..\..\
copy /y ..\..\usbip_vhci\usbip_vhci.cat
copy /y usbip_vhci.cat ..\..\
copy /y ..\..\usbip_vhci.inf
copy /y ..\..\usbip_vhci.sys
copy /y ..\..\usbip_vhci_ude\usbip_vhci_ude.cat
copy /y usbip_vhci_ude.cat ..\..\
copy /y ..\..\usbip_vhci_ude.inf
copy /y ..\..\usbip_vhci_ude.pdb
copy /y ..\..\usbip_vhci_ude.sys
