<?php
	session_start(); 
	require_once("includes/connection.php"); 

	$page = isset($_POST['page']) ? intval($_POST['page']) : 1;
	$rows = isset($_POST['rows']) ? intval($_POST['rows']) : 10;
	$offset = ($page-1)*$rows;
	
	$result = array();
	
	// Get total rows
	$rs = mysqli_query($con, "select count(*) from tempLog");
	$row = mysqli_fetch_row($rs);
	$result["total"] = $row[0];
	
	// Read tempLog without formated timeStamp 
	$rs = mysqli_query($con, "SELECT * FROM tempLog ORDER BY timeStamp DESC LIMIT $offset,$rows");
	
	$rows = array();
	while($row = mysqli_fetch_object($rs)){
		array_push($rows, $row);
	}
	$result["rows"] = $rows;

	mysqli_free_result($rs);
	mysqli_close($con);

	echo json_encode($result);
?>
