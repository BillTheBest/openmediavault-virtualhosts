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
// require("js/omv/module/admin/service/virtualhosts/window/Alias.js")

Ext.define('OMV.module.admin.service.virtualhosts.window.AliasGrid', {
    extend : 'Ext.grid.Panel',
    uses   : 'OMV.module.admin.service.virtualhosts.window.Alias',

    height : 200,
    stripeRows: true,

    columns : [{
        text     : _("Alias"),
        flex     : 1,
        sortable : false,
        dataIndex: 'alias_url'
    },{
        text     : _("Path"),
        flex     : 1,
        sortable : false,
        dataIndex: 'alias_path'
    }],

    initComponent : function() {
        var me = this;

        me.store = Ext.create('Ext.data.ArrayStore', {
            fields : [
                'alias_url',
                'alias_path'
            ]
        });

        me.dockedItems = [];
        me.dockedItems.push(Ext.widget({
            xtype: "toolbar",
            dock: "top",
            items: me.getTopToolbarItems(me)
        }));

        me.callParent(arguments);

        var selModel = me.getSelectionModel();
        selModel.on('selectionchange', function(model, records) {
            var deleteButton = me.queryById(me.getId() + "-delete");

            if (records.length > 0)
                deleteButton.enable();
            else
                deleteButton.disable();
        });
    },

    getTopToolbarItems : function() {
        var me = this;

        return [{
            id      : me.getId() + "-add",
            xtype   : "button",
            text    : _("Add"),
            icon    : "images/add.png",
            iconCls : Ext.baseCSSPrefix + "btn-icon-16x16",
            handler : me.onAddButton,
            scope   : me
        },{
            id       : me.getId() + "-delete",
            xtype    : "button",
            text     : _("Delete"),
            icon     : "images/delete.png",
            iconCls  : Ext.baseCSSPrefix + "btn-icon-16x16",
            handler  : me.onDeleteButton,
            scope    : me,
            disabled : true
        }];
    },

    onAddButton : function() {
        var me = this;

        Ext.create("OMV.module.admin.service.virtualhosts.window.Alias", {
            store : me.store
        }).show();
    },

    onDeleteButton : function() {
        var me = this;
        var records = me.getSelectionModel().getSelection();

        if (records.length > 0)
            me.store.remove(records[0]);
    }
});
