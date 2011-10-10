<?php
	
	///for mySql include db.inc.php 
 	include_once("db.inc.php");
	/// for mysqlLite include sqlitedb.inc.php
	///	include_once("sqlitedb.inc.php");

	$db=new DB();
	$db->open();


	$qry="CREATE TABLE mytable ( name varchar(255) default NULL,  age int(3) unsigned default NULL)";
	$db->query($qry);

	$qry="INSERT INTO mytable (name, age) VALUES ('harish', 23)";
	$db->query($qry);
	if($db->affectedRows())
		echo "Row inserted Succesfully<br>";
	$qry="INSERT INTO mytable (name, age) VALUES ('sriram', 23)";
	$db->query($qry);
	if($db->affectedRows())
		echo "Row inserted Succesfully<br>";

	$qry="SELECT * FROM mytable";
	$db->query($qry);

	$total_records=$db->numRows(); //gets the total records fetched ny query

	while($row=$db->fetchObject())
	{
		 echo $row->name."<br>";
	}

	/* OR we can use.
		
	while($row=$db->fetchArray())
	{
		 echo $row[first_name]."<br>";
	}
	*/

?>