<?php

/**
 * Copyright (C) 2013 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

require_once"openmediavault/module.inc";
require_once"openmediavault/config.inc";
require_once"openmediavault/error.inc";
require_once"openmediavault/util.inc";
require_once"openmediavault/initscript.inc";

class OMVModuleVirtualHosts extends OMVModuleServiceAbstract implements OMVINotifyListener {

	/**
	 * Get the module name.
	 */
	public function getName()
	{
		return "virtualhosts";
	}

	/**
	 * Create and update the configuration
	 */
	public function applyConfig()
	{
	    $cmd = "sudo omv-mkconf virtualhosts 2>&1";

        if (0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
                $cmd, implode("\n", $output));
	}

    /**
     * Start the managed service
     */
    public function startService()
    {
        //$initd = new OMVSysVInitScript('apache2', true);
        //$initd->invoke('graceful');
    }

    /**
     * Stop the managed service
     */
    public function stopService()
    {
        //$initd = new OMVSysVInitScript('apache2', false);
        //$initd->invoke('graceful');
    }

	/**
	 * Bind listeners.
	 */
	public function bindListeners(OMVNotifyDispatcher $dispatcher) {
	    $dispatcher->addListener(
            OMV_NOTIFY_CREATE | OMV_NOTIFY_MODIFY | OMV_NOTIFY_DELETE,
            "org.openmediavault.services.virtualhosts",
            array($this, "setDirty"));
	}
}

// Register module
$moduleMgr = &OMVModuleMgr::getInstance();
$moduleMgr->registerModule(new OMVModuleVirtualHosts());
