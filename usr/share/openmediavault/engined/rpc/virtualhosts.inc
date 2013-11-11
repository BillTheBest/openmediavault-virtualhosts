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

require_once "openmediavault/object.inc";
require_once "openmediavault/error.inc";
require_once "openmediavault/util.inc";
require_once "openmediavault/rpc.inc";
require_once "openmediavault/notify.inc";

class OMVRpcServiceVirtualHosts extends OMVRpcServiceAbstract
{
    private $_defaultServerNamePort = null;

    private function getXPath()
    {
        return "//services/virtualhosts";
    }

    private function getVirtualHostsXPath()
    {
        return sprintf("%s/virtual_hosts", $this->getXPath(), $uuid);
    }

    private function getVirtualHostXPathByUuid($uuid)
    {
        return sprintf("%s/virtual_host[uuid='%s']", $this->getVirtualHostsXPath(), $uuid);
    }

    public function getName()
    {
        return 'VirtualHosts';
    }

    public function initialize()
    {
        $this->registerMethod('getSettings');
        $this->registerMethod('setSettings');
        $this->registerMethod('getList');

        // CRUD
        $this->registerMethod('get');
        $this->registerMethod('set');
        $this->registerMethod('delete');
    }

    public function getSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Get configuration object
        $object = $xmlConfig->get($this->getXPath());
        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath());

        // Modify result data
        $settingsObject['enable']                   = boolval($object['enable']);
        $settingsObject['server_name_default_port'] = intval($object['server_name_default_port']);

        return $settingsObject;
    }

    public function setSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable"                   : { "type" : "boolean" },
                "server_name_default_port" : { "type" : "integer" }
            }
        }');

        // Prepare configuration data
        $object = array(
            "enable"                   => array_boolval($params, 'enable'),
            "server_name_default_port" => $params['server_name_default_port']
        );

        // Set configuration object
        $xPath = sprintf("%s/enable", $this->getXPath(), $object['enable']);
        if (false === $xmlConfig->replace($xPath, $object['enable']))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED,
                $this->$xPath);

        $xPath = sprintf("%s/server_name_default_port", $this->getXPath());
        if (false === $xmlConfig->replace($xPath, $object['server_name_default_port']))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED,
                $this->$xPath);

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, "org.openmediavault.services.virtualhosts", $object);

        return $object;
    }

    function getList($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "start"     : { "type" : "integer" },
                "limit"     : {' . $GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'] . '},
                "sortfield" : {' . $GLOBALS['OMV_JSONSCHEMA_SORTFIELD'] . '},
                "sortdir"   : {' . $GLOBALS['OMV_JSONSCHEMA_SORTDIR'] . '}
            }
        }');

        // Get configuration object
        $xPath = sprintf("%s/virtual_host/uuid", $this->getVirtualHostsXPath());
        $uuids = $xmlConfig->getList($xPath);

        if (is_null($uuids))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $xPath);

        $virtualHosts = array();

        foreach ($uuids as $uuid) {
            try {
                $virtualHosts[] = $this->get(array('uuid' => $uuid), $context);
            } catch (Exception $e) {
                throw $e;
            }
        }

        return $this->applyFilter(
            $virtualHosts,
            $params['start'],
            $params['count'],
            $params['sortfield'],
            $params['sortdir']
        );
    }

    function get($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : {' . $GLOBALS['OMV_JSONSCHEMA_UUID_NONE'] . '}
            }
        }');

        $virtualHost = $xmlConfig->get($this->getVirtualHostXPathByUuid($params['uuid']));

        if (is_null($virtualHost))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $xPath);

        // General
        $virtualHost['enable']                       = boolval($virtualHost['enable']);
        $virtualHost['host_type']                    = intval($virtualHost['host_type']);
        $virtualHost['port']                         = intval($virtualHost['port']);
        $virtualHost['server_name_use_default_port'] = boolval($virtualHost['server_name_use_default_port']);

        // Aliases
        $aliases = array();

        $aliasXPath = sprintf("%s/alias", $this->getVirtualHostXPathByUuid($virtualHost['uuid']));
        $virtualHostAliases = $xmlConfig->getList($aliasXPath);

        if (!empty($virtualHostAliases)) {
            foreach ($virtualHostAliases as $alias) {
                $a = new \stdClass();
                $a->alias_url  = $alias['alias_url'];
                $a->alias_path = $alias['alias_path'];
                $aliases[] = $a;
            }
        }

        $virtualHost['aliases'] = $aliases;
        unset($virtualHost['alias']);

        // Options
        $virtualHost['options_exec_cgi']                = boolval($virtualHost['options_exec_cgi']);
        $virtualHost['options_follow_symlinks']         = boolval($virtualHost['options_follow_symlinks']);
        $virtualHost['options_includes']                = boolval($virtualHost['options_includes']);
        $virtualHost['options_indexes']                 = boolval($virtualHost['options_indexes']);
        $virtualHost['options_multi_views']             = boolval($virtualHost['options_multi_views']);
        $virtualHost['options_symlinks_if_owner_match'] = boolval($virtualHost['options_symlinks_if_owner_match']);

        // Allow Override
        $virtualHost['allow_override_auth_config'] = boolval($virtualHost['allow_override_auth_config']);
        $virtualHost['allow_override_file_info']   = boolval($virtualHost['allow_override_file_info']);
        $virtualHost['allow_override_indexes']     = boolval($virtualHost['allow_override_indexes']);
        $virtualHost['allow_override_limit']       = boolval($virtualHost['allow_override_limit']);
        $virtualHost['allow_override_options']     = boolval($virtualHost['allow_override_options']);

        // Get full path for presentation
        $xPath = sprintf("//system/shares/sharedfolder[uuid='%s']", $virtualHost['document_root']);
        $sharedFolder = $xmlConfig->get($xPath);

        $xPath = sprintf("//system/fstab/mntent[uuid='%s']", $sharedFolder['mntentref']);
        $mountPoint = $xmlConfig->get($xPath);

        $virtualHost['document_root_full_path'] = sprintf("%s/%s",
            $mountPoint['dir'],
            $sharedFolder['reldirpath']);

        // Build possible URLs
        $urls = array();

        if ($virtualHost['host_type'] != 1) {
            $urls[] = $this->buildHref('!domain!', $virtualHost['port']);
        }

        if ($virtualHost['host_type'] > 0) {
            $port = $virtualHost['port'];

            if ($virtualHost['server_name_use_default_port'])
                $port = $this->getDefaultServerNamePort();

            $urls[] = $this->buildHref($virtualHost['server_name'], $port);
        }

        $virtualHost['urls'] = $urls;

        return $virtualHost;
    }

    public function set($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid"                         : {' . $GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'] . '},
                "enable"                       : { "type" : "boolean" },
                "document_root"                : {' . $GLOBALS['OMV_JSONSCHEMA_UUID'] . '},
                "host_type"                    : { "type" : "integer" },
                "port"                         : { "type" : "integer" },
                "server_name"                  : { "type" : "string" },
                "server_name_use_default_port" : { "type" : "boolean" },

                "aliases" : {
                    "type"     : "array",
                    "optional" : true,
                    "items"    : {
                        "type"       : "object",
                        "optional"   : true,
                        "properties" : {
                            "alias_path" : {
                                "type"     : "string",
                                "optional" : false
                            },
                            "alias_url" : {
                                "type"     : "string",
                                "optional" : false
                            }
                        }
                    }
                },

                "options_exec_cgi"                : { "type" : "boolean" },
                "options_follow_symlinks"         : { "type" : "boolean" },
                "options_includes"                : { "type" : "boolean" },
                "options_indexes"                 : { "type" : "boolean" },
                "options_multi_views"             : { "type" : "boolean" },
                "options_symlinks_if_owner_match" : { "type" : "boolean" },

                "allow_override_auth_config" : { "type" : "boolean" },
                "allow_override_file_info"   : { "type" : "boolean" },
                "allow_override_indexes"     : { "type" : "boolean" },
                "allow_override_limit"       : { "type" : "boolean" },
                "allow_override_options"     : { "type" : "boolean" },

                "extra_options" : { "type" : "string" }
            }
        }');

        $virtualHost = array(
            'uuid'                         => empty($params['uuid']) ? OMVUtil::uuid() : $params['uuid'],
            'enable'                       => boolval($params['enable']),
            'document_root'                => $params['document_root'],
            'host_type'                    => intval($params['host_type']),
            'port'                         => intval($params['port']),
            'server_name'                  => $params['server_name'],
            'server_name_use_default_port' => boolval($params['server_name_use_default_port']),

            'alias' => $params['aliases'],

            'options_exec_cgi'                => boolval($params['options_exec_cgi']),
            'options_follow_symlinks'         => boolval($params['options_follow_symlinks']),
            'options_includes'                => boolval($params['options_includes']),
            'options_indexes'                 => boolval($params['options_indexes']),
            'options_multi_views'             => boolval($params['options_multi_views']),
            'options_symlinks_if_owner_match' => boolval($params['options_symlinks_if_owner_match']),

            'allow_override_auth_config' => boolval($params['allow_override_auth_config']),
            'allow_override_file_info'   => boolval($params['allow_override_file_info']),
            'allow_override_indexes'     => boolval($params['allow_override_indexes']),
            'allow_override_limit'       => boolval($params['allow_override_limit']),
            'allow_override_options'     => boolval($params['allow_override_options']),

            'extra_options' => $params['extra_options']
        );

        if (empty($params['aliases']))
            unset($virtualHost['alias']);
        else
            $virtualHost['alias'] = $params['aliases'];

        if ($virtualHost['host_type'] != 1 ||
            $virtualHost['host_type'] == 1 &&
            !$virtualHost['server_name_use_default_port'])
        {
            if ($this->portIsUsed($virtualHost['port'], $virtualHost['uuid']))
                throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_INVALID,
                    "Selected port is already used");
        } else {
            unset($virtualHost['port']);
        }

        $success = false;

        $xPath = null;

        if (empty($params['uuid']))
            $success = $xmlConfig->set($xPath = $this->getVirtualHostsXPath(),
                array('virtual_host' => $virtualHost),
                $virtualHost);
        else
            $success = $xmlConfig->replace($xPath = $this->getVirtualHostXPathByUuid($params['uuid']),
                $virtualHost);

        if (!$success)
            throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_NOT_FOUND,
                $xPath);

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(empty($params['uuid']) ? OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
            "org.openmediavault.services.virtualhosts", $virtualHost);

        return $virtualHost;
    }

    public function delete($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : {' . $GLOBALS['OMV_JSONSCHEMA_UUID_NONE'] . '}
            }
        }');

        $virtualHost = $xmlConfig->delete($this->getVirtualHostXPathByUuid($params['uuid']));

        if (is_null($virtualHost))
            throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_NOT_FOUND,
                $this->getVirtualHostXPathByUuid($params['uuid']));

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_DELETE,
            "org.openmediavault.services.virtualhosts", $virtualHost);
    }

    public function portIsUsed($port, $uuid)
    {
        global $xmlConfig;

        $xPath = sprintf("%s/virtual_host[port='%s']", $this->getVirtualHostsXPath(), $port);
        $virtualHost = $xmlConfig->get($xPath);

        if (!empty($virtualHost))
            if ($virtualHost['uuid'] != $uuid)
                return true;

        return false;
    }

    private function buildHref($domain, $port)
    {
        $url = sprintf("http://%s:%s", $domain, $port);
        return "<a href='$url' target='_blank'>$url</a>";
    }

    private function getDefaultServerNamePort()
    {
        if (!$this->_defaultServerNamePort) {
            global $xmlConfig;
            $xPath = sprintf("%s/server_name_default_port", $this->getXPath());
            $this->_defaultServerNamePort = $xmlConfig->get($xPath);
        }

        return $this->_defaultServerNamePort;
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceVirtualHosts());
