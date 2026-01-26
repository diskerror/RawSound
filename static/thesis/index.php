<?php

$fileMime = 'application/pdf';
$fileName = 'ReidWoodburyThesis.pdf';
$fileContent = file_get_contents($fileName);
$fileTime = filemtime($fileName);

require '../file.php';
