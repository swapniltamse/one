/* -------------------------------------------------------------------------- */
/* Copyright 2002-2011, OpenNebula Project Leads (OpenNebula.org)             */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License"); you may    */
/* not use this file except in compliance with the License. You may obtain    */
/* a copy of the License at                                                   */
/*                                                                            */
/* http://www.apache.org/licenses/LICENSE-2.0                                 */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/* -------------------------------------------------------------------------- */

/** HISTORY_LENGTH currently ignored on server, but it doesn't harm to have it**/
var HISTORY_LENGTH=40;
var GRAPH_AUTOREFRESH_INTERVAL=60000; //60 secs

var graph1 = {
    title : "graph1",
    monitor_resources : "cpu_usage,used_cpu,max_cpu",
    history_length : HISTORY_LENGTH
};

var graph2 = {
    title : "graph2",
    monitor_resources : "mem_usage,used_mem,max_mem",
    history_length : HISTORY_LENGTH
};

var graph3 = {
    title : "graph3",
    monitor_resources : "total,active,error",
    history_length : HISTORY_LENGTH
};

var graph4 = {
    title : "graph4",
    monitor_resources : "net_tx,net_rx",
    history_length : HISTORY_LENGTH
};

var dashboard_tab_content =
'<table id="dashboard_table">\
<tr>\
<td style="width:40%">\
<table id="information_table" style="width:100%">\
  <tr>\
    <td>\
      <div class="panel">\
        <h3>Summary of resources</h3>\
        <div class="panel_info">\
\
          <table class="info_table">\
            <tr>\
              <td class="key_td">Hosts (total/active)</td>\
              <td class="value_td"><span id="total_hosts"></span><span id="active_hosts" class="green"></span></td>\
            </tr>\
            <tr>\
              <td class="key_td">Groups</td>\
              <td class="value_td"><span id="total_groups"></span></td>\
            </tr>\
            <tr>\
              <td class="key_td">VM Templates (total/public)</td>\
              <td class="value_td"><span id="total_templates"></span><span id="public_templates"></span></td>\
            </tr>\
            <tr>\
              <td class="key_td">VM Instances (total/<span class="green">running</span>/<span class="red">failed</span>)</td>\
              <td class="value_td"><span id="total_vms"></span><span id="running_vms" class="green"></span><span id="failed_vms" class="red"></span></td>\
            </tr>\
            <tr>\
              <td class="key_td">Virtual Networks (total/public)</td>\
              <td class="value_td"><span id="total_vnets"></span><span id="public_vnets"></span></td>\
            </tr>\
            <tr>\
              <td class="key_td">Images (total/public)</td>\
              <td class="value_td"><span id="total_images"></span><span id="public_images"></span></td>\
            </tr>\
            <tr>\
              <td class="key_td">Users</td>\
              <td class="value_td"><span id="total_users"></span></td>\
            </tr>\
            <tr>\
              <td class="key_td">ACL Rules</td>\
              <td class="value_td"><span id="total_acls"></span></td>\
            </tr>\
          </table>\
\
        </div>\
      </div>\
    </td>\
  </tr>\
  <tr>\
    <td>\
      <div class="panel">\
        <h3>Quickstart</h3>\
        <form id="quickstart_form"><fieldset>\
          <table style="width:100%;"><tr style="vertical-align:middle;"><td style="width:70%">\
          <label style="font-weight:bold;width:40px;height:10em;">New:</label>\
          <input type="radio" name="quickstart" value="Host.create_dialog">Host</input><br />\
          <input type="radio" name="quickstart" value="VM.create_dialog">VM Instance</input><br />\
          <input type="radio" name="quickstart" value="Template.create_dialog">VM Template</input><br />\
          <input type="radio" name="quickstart" value="Network.create_dialog">Virtual Network</input><br />\
          <input type="radio" name="quickstart" value="Image.create_dialog">Image</input><br />\
          <input type="radio" name="quickstart" value="User.create_dialog">User</input><br />\
          <input type="radio" name="quickstart" value="Group.create_dialog">Group</input><br />\
          <input type="radio" name="quickstart" value="Acl.create_dialog">Acl</input><br />\
          </td></tr></table>\
      </div>\
    </td>\
  </tr>\
</table>\
</td>\
<td style="width:60%">\
<table id="historical_table" style="width:100%">\
  <tr>\
    <td>\
      <div class="panel">\
        <h3>Historical monitoring information</h3>\
        <div class="panel_info">\
          <table class="info_table">\
            <tr><td class="key_td graph_td">Hosts CPU</td>\
                <td class="graph_td" id="graph1_legend"></td></tr>\
            <tr><td id="graph1" colspan="2">'+spinner+'</td></tr>\
            <tr><td class="key_td graph_td">Hosts memory</td>\
                <td class="graph_td" id="graph2_legend"></td></tr>\
            <tr><td id="graph2" colspan="2">'+spinner+'</td></tr>\
            <tr><td class="key_td graph_td">Total VM count</td>\
                <td class="graph_td" id="graph3_legend"></td></tr>\
            <tr><td id="graph3" colspan="2">'+spinner+'</td></tr>\
            <tr><td class="key_td graph_td">VM Network stats</td>\
                <td class="graph_td" id="graph4_legend"></td></tr>\
            <tr><td id="graph4" colspan="2">'+spinner+'</td></tr>\
          </table>\
        </div>\
      </div>\
    </td>\
  </tr>\
</table>\
</td>\
</tr></table>';

var dashboard_tab = {
    title: 'Dashboard',
    content: dashboard_tab_content
}

Sunstone.addMainTab('dashboard_tab',dashboard_tab);

function plot_global_graph(data,info){
    var context = $('#historical_table',main_tabs_context);
    var id = info.title;
    var monitoring = data.monitoring;
    var serie;
    var series = [];
    var width = ($(window).width()-129)*48/100;
    var mon_count = 0;
    var labels_array = info.monitor_resources.split(',');

    $('#'+id,context).html('<div id="'+id+'_graph" style="height:70px;width:'+width+'px;margin-bottom:10px;"><div>');

    for (var i=0; i<labels_array.length; i++) {
        serie = {
            label: labels_array[i],
            data: monitoring[labels_array[i]]
        };
        series.push(serie);
        mon_count++;
    };

    var options = {
        legend : {
            show : true,
            noColumns: mon_count,
            container: $('#'+id+'_legend')
        },
        xaxis : {
            tickFormatter: function(val,axis){
                return pretty_time_axis(val);
            },
        },
        yaxis : { labelWidth: 20 }
    }

    switch (id){
    case "graph2":
    case "graph4":
        options["yaxis"]["tickFormatter"] = function(val,axis) {
            return humanize_size(val);
        }
    }

    $.plot($('#'+id+'_graph',context),series,options);
}

function quickstart_setup(){

    $('#dashboard_table #quickstart_form input',main_tabs_context).click(function(){
        Sunstone.runAction($(this).val());
    });
}

function graph_autorefresh(){
    setInterval(function(){
        refresh_graphs();
    },GRAPH_AUTOREFRESH_INTERVAL+someTime());

}

function refresh_graphs(){
    Sunstone.runAction("Host.monitor_all", graph1);
    Sunstone.runAction("Host.monitor_all", graph2);
    Sunstone.runAction("VM.monitor_all", graph3);
    Sunstone.runAction("VM.monitor_all", graph4);
}

$(document).ready(function(){
    //Dashboard link listener
    $("#dashboard_table h3 a",main_tabs_context).live("click", function (){
        var tab = $(this).attr('href');
        showTab(tab);
        return false;
    });

    emptyDashboard();

    quickstart_setup();

    refresh_graphs();
    graph_autorefresh();

});

//puts the dashboard values into "retrieving"
function emptyDashboard(){
    $("#dashboard_tab .value_td span",main_tabs_context).html(spinner);
}


function updateDashboard(what,json_info){
    var db = $('#dashboard_tab',main_tabs_context);
    switch (what){
    case "hosts":
        var total_hosts=json_info.length;
        var active_hosts=0;
        $.each(json_info,function(){
            if (parseInt(this.HOST.STATE) < 3){
                active_hosts++;}
        });
        $('#total_hosts',db).html(total_hosts+'&nbsp;/&nbsp;');
        $('#active_hosts',db).html(active_hosts);
        break;
    case "groups":
        var total_groups=json_info.length;
        $('#total_groups',db).html(total_groups);
        break;
    case "vms":
        var total_vms=json_info.length;
        var running_vms=0;
            failed_vms=0;
        $.each(json_info,function(){
            vm_state = parseInt(this.VM.STATE);
            if (vm_state == 3){
                running_vms++;
            }
            else if (vm_state == 7) {
                failed_vms++;
            }
        });
        $('#total_vms',db).html(total_vms+'&nbsp;/&nbsp;');
        $('#running_vms',db).html(running_vms+'&nbsp;/&nbsp;');
        $('#failed_vms',db).html(failed_vms);
        break;
    case "vnets":
        var public_vnets=0;
        var total_vnets=json_info.length;
        $.each(json_info,function(){
            if (parseInt(this.VNET.PUBLIC)){
                public_vnets++;}
        });
        $('#total_vnets',db).html(total_vnets+'&nbsp;/&nbsp;');
        $('#public_vnets',db).html(public_vnets);
        break;
    case "users":
        var total_users=json_info.length;
        $('#total_users',db).html(total_users);
        break;
    case "images":
        var total_images=json_info.length;
        var public_images=0;
        $.each(json_info,function(){
            if (parseInt(this.IMAGE.PUBLIC)){
                public_images++;}
        });
        $('#total_images',db).html(total_images+'&nbsp;/&nbsp;');
        $('#public_images',db).html(public_images);
        break;
    case "templates":
        var total_templates=json_info.length;
        var public_templates=0;
        $.each(json_info,function(){
            if (parseInt(this.VMTEMPLATE.PUBLIC)){
                public_templates++;
            }
        });
        $('#total_templates',db).html(total_templates+'&nbsp;/&nbsp;');
        $('#public_templates',db).html(public_templates);
        break;
    case "acls":
        var total_acls=json_info.length;
        $('#total_acls',db).html(total_acls);
        break;
    }
}
