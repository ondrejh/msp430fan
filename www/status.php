<div class="contentTitle"><h1>Device status:</h1></div>
<div class="contentText">
<p>
	<META HTTP-EQUIV="refresh" CONTENT="3">

	<?php
		echo "PHP MySQL dump test page:<br>\n";

		$con = mysqli_connect("localhost","root","1234","ebio");

		// check connection
		if (mysqli_connect_errno($con))
		{
			echo "Failed to connect to MySQL: ".mysqli_connect_error();
		}

		for ($id=1;$id<6;$id++)
		{
			$sql = "SELECT value FROM temp WHERE id=$id";
			//echo $sql."<br>\n";

			if ($result=mysqli_query($con,$sql))
			{
				// get field information
				$fielddata=mysqli_fetch_row($result);
				printf("Temp.$id: %s<br>\n",$fielddata[0]);

				// free result set
				mysqli_free_result($result);
			}
		}

		$sql = "SELECT * FROM heating";
		if ($result=mysqli_query($con,$sql))
		{
			// get data
			$fielddata=mysqli_fetch_row($result);
			printf("Heating status: %s<br>\n",$fielddata[1]);

			// free result set
			mysqli_free_result($result);
		}

		mysqli_close($con);
		echo "Das ende<br>\n";
	?>

</p>       
<p>&nbsp;</p>
<p><a href="index.html">(read more)</a></p>
</div>