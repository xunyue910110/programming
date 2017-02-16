 select c.value || '\' || d.instance_name ||
                   '_ora_' || a.spid || '.trc' trace
  from v$process a, v$session b, v$parameter c, v$instance d
 where a.addr = b.paddr
   and b.audsid = userenv('sessionid')
   and c.name = 'user_dump_dest'
/

