<?php

	//include("connect.php"); 	
	
	///$link=Connection();

	//$result=mysql_query("SELECT * FROM `tempLog` ORDER BY `timeStamp` DESC",$link);
	
    session_start(); 
    require_once("includes/connection.php"); 

	$sql="SELECT * FROM tempLog ORDER BY timeStamp DESC"; 
					  
	$result=mysqli_query($con, $sql); 
	//while($row=mysqli_fetch_array($query)){ 

?>


<?php 
	  if($result!==FALSE){
	     echo("[");
	     while($row = mysqli_fetch_array($result)) {
	        //printf("<tr><td> &nbsp;%s </td><td> &nbsp;%s&nbsp; </td><td> &nbsp;%s&nbsp; </td></tr>", $row["timeStamp"], $row["temperature"], $row["humidity"]);
		echo json_encode($row);
		echo(",");
		//echo("<br>");
		//var_dump(json_decode($row));

	     }
	     echo("{}");
	     echo("]");
	     mysqli_free_result($result);
	     mysqli_close($con);
	  }
?>
 