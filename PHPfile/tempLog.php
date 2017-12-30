<?php

      session_start(); 
      require_once("includes/connection.php"); 

	// Read android data
       $sql="SELECT * FROM androidData ORDER BY id DESC LIMIT 1"; 
					  
	$resultAndroid=mysqli_query($con, $sql);
	$row=mysqli_fetch_array($resultAndroid);
	$OnOff = $row["name"];
	mysqli_free_result($resultAndroid);

	//Read newest Temperature
	$sql="SELECT temperature FROM tempLog ORDER BY timeStamp DESC LIMIT 1"; 
	$resultTemp=mysqli_query($con, $sql);
	$rowTemp=mysqli_fetch_array($resultTemp);
	$newTemp = $rowTemp["temperature"];
	mysqli_free_result($resultTemp);

	// Read tempLog with formated timeStamp for 30 min
	$sql="SELECT temperature, humidity, UNIX_TIMESTAMP(timeStamp) AS datetime FROM tempLog ORDER BY timeStamp DESC LIMIT 1800"; 
				  
	$result=mysqli_query($con, $sql);

	$rows = array();
	$table = array();

	$table["cols"] = array(
	 array(
  	  "label" => "Date Time", 
  	  "type" => "datetime" //"string" //
 	  ),

	array(
	  "label" => "Humidity (%)", 
	  "type" => "number"
 	  ),

 	array(
	  "label" => "Temperature (C)", 
	  "type" => "number"
 	  )
	
	); 
      
	while($row = mysqli_fetch_array($result))
	{

	 $sub_array = array();

	 $datetime = explode(" -:", $row["datetime"]);	
	

	 $sub_array[] =  array(
	      "v" => 'Date(' . $datetime[0] . '000)'
	      //"v" => $row["datetime"]
	      //"v" => $datetime
	     );

	$sub_array[] =  array(
	      "v" => $row["humidity"]
	     );

	$sub_array[] =  array(
	      "v" => $row["temperature"]
	     );

	 $rows[] =  array(
	     "c" => $sub_array
	    );
	//echo $row["temperature"];

	}
	$table['rows'] = $rows;

	//echo json_encode($rows);
	$jsonTable = json_encode($table);

	//echo $jsonTable;

	mysqli_free_result($result);

	//close the db connection
	//mysqli_close($con);

?>

<html>
   <head>
	<meta http-equiv="refresh" content="30">
	<title>Sauna Sensor Data</title>
	<!-- Latest compiled and minified CSS -->
	<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.2/css/bootstrap.min.css">
	<link rel="stylesheet" type="text/css" href="http://www.jeasyui.com/easyui/themes/default/easyui.css">
	<link rel="stylesheet" type="text/css" href="http://www.jeasyui.com/easyui/themes/icon.css">
	<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
	<!-- jQuery library -->
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
  	<!--script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"--><!--/script-->
	<!--script type="text/javascript" src="http://code.jquery.com/jquery-1.4.4.min.js"--><!--/script-->
	<!-- EasyUI library -->
	<script type="text/javascript" src="http://www.jeasyui.com/easyui/jquery.easyui.min.js"></script>
	<!-- Popper JS -->
	<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.6/umd/popper.min.js"></script>
	<!-- Latest compiled JavaScript -->
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.2/js/bootstrap.min.js"></script>

	<script type="text/javascript">
   	google.charts.load('current', {'packages':['corechart']});
   	google.charts.setOnLoadCallback(drawChart);
   	function drawChart()
   	{
    		var data = new google.visualization.DataTable(<?php echo $jsonTable; ?>);

    		var options = {
     			title:'Sensors Data',
     			legend:{position:'bottom'},
     			chartArea:{width:'90%', height:'65%'}
    			};

    		var chart = new google.visualization.LineChart(document.getElementById('line_chart'));

    		chart.draw(data, options);
   	}
  	</script>
  	<style>
  	    .page-wrapper
  	    {
   		width:1000px;
   		margin-top:0 auto;
  	    }

	    .datagrid-view,
	    .datagrid-pager,
	    .datagrid-footer,
	    .datagrid-footer-inner,
	    .datagrid-cell-group,
	    .datagrid-header-rownumber,
	    .datagrid-cell-rownumber,
	    .datagrid-cell
	    {
    		font-size:13px;
	    }
	    .datagrid-header .datagrid-cell span {
  		font-size: 13px;
		font-weight: bold;
	    }
	    .pagination,
	    .pagination-page-list,
	    .pagination-num,
	    .pagination-info
	    {
    		font-size:13px;
	    }

  	</style>
   </head>
<body>
   <h3 align="center" >Sauna data readings</h3>
   <p/>
   <div class="row"> 
	<div class="col-md-6" align="right">
          <h4>Current Status: </h4>
	</div>
	<div class="col-md-1 <?php if ($OnOff == 0) echo 'btn-danger'; else echo 'btn-success'; ?>" align="left">
	   <h4> 
              <?php
		    if($OnOff == 0)
		    {
			echo("Off");
		    }else	
		    {
			echo("On");
		    }
              ?> 
	   </h4>
	</div>
   </div >
   <p/>

   <div style="text-align:center">
   <form action="webButton.php" method="post">
   <?php
	if($OnOff == 1){
	    if($newTemp > 90){
		// write 0 (off) to database
    		$query2 = "INSERT INTO androidData (name) VALUES ('0')";
    		mysqli_query($con,$query2);
 	    }
   ?>
   	<div>
	    <input type="submit" name="0" class="btn btn-danger" value="OFF" style="width: 6em;height: 4em"  />
   	</div>
   <?php
	}
	else {
	   if($newTemp > 80){
   ?>
   	<div>
	    <input type="button" disable name="1" class="btn" value="ON" style="width: 6em;height: 4em"  />
   	</div>
   <?php
	    }
	    else {
   ?>
   	<div>
	    <input type="submit" name="1" class="btn btn-success" value="ON" style="width: 6em;height: 4em"  />
   	</div>
   <?php
	    }
	}
   ?> 
   </form>
   </div >
   <?php  
	if($newTemp > 80){
		$message = "*** HIGH TEMPERATURE ALERT !!!";
		echo "<script type='text/javascript'>alert('$message');</script>";		
	}
   ?>

   <table border="1" cellspacing="1" cellpadding="1" >
	<td width="30%" align="center">
   	   <h4>Temperature / moisture sensor</h4>
		<!-- this table use data from getTempData.php to display in a paging grid -->
		<table id="tt" class="easyui-datagrid" style="width:350px;height:380px"
			url="getTempData.php"
			title="Load Data" iconCls="icon-save"
			rownumbers="true" pagination="true">
		    <thead>
			<tr>
				<th field="timeStamp" width="45%">Time</th>
				<th field="temperature" width="30%" align="right">Temperature</th>
				<th field="humidity" width="25%" align="right">Humidity</th>
			</tr>
		    </thead>
		</table>
	   <p/>
	   <div class="row">
   	   <div class="col-md-6">
		<input type="button" value="Clear data" class="btn btn-danger" onclick="window.location.href='./deleteTempLog.php'" />
   	   </div>
	   <div class="col-md-6">
		<input type="button" value="Export to Excel" class="btn btn-success" onclick="window.location.href='./excel.php'" />
	   </div>
	   </div>
	</td>
	<td width="70%" valign="top">
  	    <div class="page-wrapper">
   		<br />
   		<h2 align="center">Display Google Line Chart with JSON PHP & Mysql</h2>
   		<div id="line_chart" style="width: 100%; height: 500px"; align-items: top;></div>
  	    </div>
	</td>
   </table>
</body>
</html>
