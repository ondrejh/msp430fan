<?php
	echo "<div class=\"contentTitle\"><h1>Charts:</h1></div>\n";
	echo "<div class=\"contentText\">\n";
	echo "<p>\n";
	echo "	<META HTTP-EQUIV=\"refresh\" CONTENT=\"60\">\n";

	echo exec('cd script; ./plot.sh');
	
	echo "  <img class=\"center\" src=\"script/data.png\" alt=\"chart\">\n";
	
	echo "</p>\n";
	echo "<p>&nbsp;</p>\n";
	echo "<p><a href=\"index.html\">(read more)</a></p>\n";
	echo "</div>\n"
?>