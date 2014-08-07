<?php

$filename = "/tmp/temperature/setting.json";

$fp = fopen($filename, "r");
$setting = fread($fp, filesize($filename));
fclose($fp);

echo $setting;

?>
