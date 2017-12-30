<?php

	session_start(); 
       require_once("includes/connection.php"); 

       $sql="SELECT * FROM androidData ORDER BY id DESC LIMIT 1"; 
					  
	$result=mysqli_query($con, $sql);

?>

<html>
   <head>
      <title>Android Data</title>
   </head>
<body>
   <h1>Android data readings</h1>
   <div> 
      <p/>
      <div>
          Name: 
              <?php
                  if($result!==FALSE){
	              $row=mysqli_fetch_array($result);
                     printf($row["name"]);
	              mysqli_free_result($result);
	              mysqli_close($con);
                  }
              ?> 
      </div>
   </div>
</html>