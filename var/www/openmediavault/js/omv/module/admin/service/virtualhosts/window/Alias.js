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

// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/window/Form.js")

Ext.define("OMV.module.admin.service.virtualhosts.window.Alias", {
    extend : "OMV.workspace.window.Form",

    title           : _("Add alias"),
    hideResetButton : true,
    mode            : "local",

    getFormItems : function() {
        var me = this;

        return [{
            xtype       : "textfield",
            name        : "alias_url",
            fieldLabel  : _("URL"),
            regex       : /^\/\w+/i,
            invalidText : _("The URL must start with a '/'."),
            allowBlank  : false,
            allowNone   : false,
        },{
            xtype       : "sharedfoldercombo",
            fieldLabel  : _("Shared folder"),
            allowBlank  : true,
            allowNone   : true,
            isFormField : false,
            listeners   : {
                scope    : me,
                'select' : function(combo, records, eOpts) {
                    if (records.length === 0)
                        return;

                    var alias_path = me.findField('alias_path');

                    OMV.Rpc.request({
                        scope    : me,
                        callback : function(id, success, response) {
                            if (success)
                                alias_path.setValue(response);
                        },
                        rpcData  : {
                            service : "ShareMgmt",
                            method  : "getPath",
                            params  : {
                                uuid : records[0].get("uuid")
                            }
                        }
                    });
                }
            },
            plugins    : [{
                ptype : "fieldinfo",
                text  : _("Automatically populate the path with the path of the selected shared folder.")
            }]
        },{
            xtype       : "textfield",
            name        : "alias_path",
            fieldLabel  : _("Path"),
            regex       : /^\/[^\0]+/i,
            invalidText : _("The path must start with a '/'."),
            allowBlank  : false,
            allowNone   : false
        }];
    },

    onOkButton : function() {
        var me = this;

        if (!me.isValid())
            return;

        var url  = me.findField("alias_url").value;
        var path = me.findField("alias_path").value;

        me.store.add({
            alias_url  : url,
            alias_path : path
        });

        me.close();
    }
});
