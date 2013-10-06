function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    // call default implementation to actually install README.txt!
    component.createOperations();

    if (installer.value("os") === "win") {
        component.addOperation("CreateShortcut", "@TargetDir@/release/BioVision.exe", "@StartMenuDir@/BioVision.lnk");
		component.addOperation("CreateShortcut", "@TargetDir@/release/BioVision.exe", "@DesktopDir@/BioVision.lnk");
		component.addOperation("CreateShortcut", "@TargetDir/UninstallBioVision.exe", "@StartMenuDir@/Uninstall.lnk");
    }
	
	if(installer.value("os") === "mac") {
		
	}
}
