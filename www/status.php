<?php
	echo "<div class=\"contentTitle\"><h1>Device status:</h1></div>\n";
	echo "<div class=\"contentText\">\n";
	echo "<p>\n";
	echo "	<META HTTP-EQUIV=\"refresh\" CONTENT=\"3\">\n";

	$con = mysqli_connect("localhost","root","1234","ebio");

	// check connection
	if (mysqli_connect_errno($con))
	{
		echo "Failed to connect to MySQL: ".mysqli_connect_error();
	}
	
	// check input
	if ($_REQUEST["action"]=="stop")
	{
		// set stop request
		$sql = "UPDATE ebio.progstat SET request='STOP'";
		$result=mysqli_query($con,$sql);
		// reload page (without action='stop')
		echo "<meta http-equiv=\"refresh\" content=\"1;url=index.php\">";
	}

	// temperatures
	$t1 = "---";
	for ($id=1;$id<6;$id++)
	{
		$sql = "SELECT value FROM temp WHERE id=$id";
		//echo $sql."<br>\n";
		if ($result=mysqli_query($con,$sql))
		{
			// get field information
			$fielddata=mysqli_fetch_row($result);
			if ($id!=5)
				printf("Temperature $id: %s<br>\n",$fielddata[0]);
			else
				printf("MCU temperature: %s<br>\n",$fielddata[0]);
				
			if ($id==1) $t1=$fielddata[0];
			// free result set
			mysqli_free_result($result);
		}
	}

	// heating status
	$sql = "SELECT * FROM heating";
	if ($result=mysqli_query($con,$sql))
	{
		// get data
		$fielddata=mysqli_fetch_row($result);
		printf("Heating status: %s<br>\n",$fielddata[1]);
		printf("Fuse switch: %s<br>\n",$fielddata[2]);

		// free result set
		mysqli_free_result($result);
	}
	
	// server status
	$sql = "SELECT * FROM progstat";
	if ($result=mysqli_query($con,$sql))
	{
		// get data
		$fielddata = mysqli_fetch_row($result);
		printf("Server status: %s<br>\n",$fielddata[1]);
		
		// test if data actual (if not server stopped)
		if ($fielddata[1]!='STOPPED')
		{
			$tdif = time()-strtotime($fielddata[2]);
			// if data older than 30sec and no STOPPED status set, rewrite it
			if ($tdif<30)
			{
				$sql = "UPDATE ebio.progstat SET status='STOPPED'";
				$result=mysqli_query($con,$sql);
			}
		}
		
		// free result set
		mysqli_free_result($result);
	}
	
	printf("<div id=\"t1\">%s</div>\n",$t1);

	mysqli_close($con);
	echo "<br>\n";
	
	#stop button
	echo "<form method='get' action='index.php'>\n";
	echo "	<button class='btnstop' type='submit' name='action' value='stop'>STOP</button>\n";
	//echo "	<button class='btnstart' type='submit' name='action' value='start'>START</button>\n";
	echo "</form>\n";
	
	echo "</p>\n";
	echo "<p>&nbsp;</p>\n";
	echo "<p><a href=\"index.html\">(read more)</a></p>\n";
	echo "</div>\n"
?>