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
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/form/plugin/LinkedFields.js")
// require("js/omv/form/field/SharedFolderComboBox.js")
// require("js/omv/module/admin/service/virtualhosts/window/AliasGrid.js")

Ext.define("OMV.module.admin.service.virtualhosts.window.VirtualHost", {
    extend : "OMV.workspace.window.Form",
    uses   : [
        "OMV.workspace.window.plugin.ConfigObject",
        "OMV.form.plugin.LinkedFields",
        "OMV.module.admin.service.virtualhosts.window.AliasGrid",
    ],

    plugins : [{
        ptype : "configobject"
    },{
        ptype        : "linkedfields",
        correlations : [{
            name : [
                "server_name",
                "server_name_use_default_port"
            ],
            conditions : [{
                name  : "host_type",
                op    : ">",
                value : 0
            }],
            properties : [
                "show",
                "!allowBlank",
                "!readOnly"
            ]
        },{
            name : [
                "port"
            ],
            conditions : [{
                name  : "host_type",
                value : 1
            },{
                name  : "server_name_use_default_port",
                value : true
            }],
            properties : [
                "hide",
                "allowBlank",
                "readOnly"
            ]
        }]
    }],

    rpcService   : "VirtualHosts",
    rpcSetMethod : "set",

    height          : 600,
    hideResetButton : true,
    uuid            : null,

    getFormItems : function() {
        var me = this;

        return [{
            xtype : "fieldset",
            title : _("General"),
            items : [{
                xtype      : "checkbox",
                name       : "enable",
                fieldLabel : _("Enable"),
                checked    : true
            },{
                xtype      : "sharedfoldercombo",
                name       : "document_root",
                fieldLabel : _("Document root"),
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("The location needs to have at least read permissions for the user/group www-data")
                }]
            },{
                xtype      : "combo",
                name       : "host_type",
                fieldLabel : _("Host type"),
                queryMode  : "local",
                store      : Ext.create("Ext.data.ArrayStore", {
                    fields : [
                        "value",
                        "text"
                    ],
                    data   : [
                        [ 0, _("Port") ],
                        [ 1, _("ServerName") ],
                        [ 2, _("Both") ],
                    ]
                }),
                displayField  : "text",
                valueField    : "value",
                allowBlank    : false,
                editable      : false,
                triggerAction : "all",
                value         : 0
            },{
                xtype         : "numberfield",
                name          : "port",
                fieldLabel    : _("Port"),
                vtype         : "port",
                minValue      : 0,
                maxValue      : 65535,
                allowDecimals : false,
                allowNegative : false,
                value         : 8080
            },{
                xtype      : "textfield",
                name       : "server_name",
                fieldLabel : _("ServerName"),
                allowBlank : true,
                readOnly   : true,
                hidden     : true
            },{
                xtype      : "checkbox",
                name       : "server_name_use_default_port",
                fieldLabel : _("Use default port for ServerName"),
                checked    : true,
                readOnly   : true,
                hidden     : true,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Untick to host the ServerName with the portnumber specified in the field port.")
                }]
            }]
        },{
            xtype : "fieldset",
            title : _("Aliases"),
            items : [{
                xtype      : "gridfield",
                name       : "aliases",
                gridClass  : "OMV.module.admin.service.virtualhosts.window.AliasGrid",
            }]
        },{
            xtype    : "fieldset",
            title    : _("Options"),
            layout   : "column",
            defaults : {
                columnWidth : 0.5,
                layout      : 'form',
                border      : false,
            },
            items  : [{
                defaults : {
                    hideLabel      : true,
                    labelSeparator : ''
                },
                items : [{
                    xtype      : "checkbox",
                    name       : "options_exec_cgi",
                    boxLabel   : _("ExecCGI"),
                    checked    : false
                },{
                    xtype      : "checkbox",
                    name       : "options_follow_symlinks",
                    boxLabel   : _("FollowSymlinks"),
                    checked    : false
                },{
                    xtype      : "checkbox",
                    name       : "options_includes",
                    boxLabel   : _("Includes"),
                    checked    : false
                }]
            },{
                defaults : {
                    hideLabel      : true,
                    labelSeparator : ''
                },
                items : [{
                    xtype      : "checkbox",
                    name       : "options_indexes",
                    boxLabel   : _("Indexes"),
                    checked    : false
                },{
                    xtype      : "checkbox",
                    name       : "options_multi_views",
                    boxLabel   : _("MultiViews"),
                    checked    : false
                },{
                    xtype      : "checkbox",
                    name       : "options_symlinks_if_owner_match",
                    boxLabel   : _("SymlinksIfOwnerMatch"),
                    checked    : false
                }]
            }]
        },{
            xtype : "fieldset",
            title : _("AllowOverride"),
            layout   : "column",
            defaults : {
                columnWidth : 0.5,
                layout      : 'form',
                border      : false,
            },
            items : [{
                defaults : {
                    hideLabel      : true,
                    labelSeparator : ''
                },
                items : [{
                    xtype      : "checkbox",
                    name       : "allow_override_auth_config",
                    boxLabel   : _("AuthConfig"),
                    checked    : false
                },{
                    xtype      : "checkbox",
                    name       : "allow_override_file_info",
                    boxLabel   : _("FileInfo"),
                    checked    : false
                },{
                    xtype      : "checkbox",
                    name       : "allow_override_indexes",
                    boxLabel   : _("Indexes"),
                    checked    : false
                }]
            },{
                defaults : {
                    hideLabel      : true,
                    labelSeparator : ''
                },
                items : [{
                    xtype      : "checkbox",
                    name       : "allow_override_limit",
                    boxLabel   : _("Limit"),
                    checked    : false
                },{
                    xtype      : "checkbox",
                    name       : "allow_override_options",
                    boxLabel   : _("Options"),
                    checked    : false
                }]
            }]
        },{
            xtype : "fieldset",
            title : _("Extra options"),
            items : [{
                xtype      : "textarea",
                name       : "extra_options",
                fieldLabel : _("Extra options"),
                allowBlank : true
            }]
        }];
    }
});
