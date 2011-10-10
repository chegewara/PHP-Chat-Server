<?php


     /*
     ###############################################
     ####                                       ####
     ####    Author : Harish Chauhan            ####
     ####    Date   : 29 Sep,2004               ####
     ####    Updated:                           ####
     ####                                       ####
     ###############################################

     */

	class DB
	{
		// Connection parameters 
			
		var $database = '';///database i.e ../database/first_step.db
		var $persistent = false;
	  
		
		// Database connection handle 
		var $conn = NULL;

		// Query result 
		var $result = false;

		function DB()
		{
		}
		function open()
		{
			// Choose the appropriate connect function 
			if ($this->persistent) {
				$func = 'sqlite_popen';
			} else {
				$func = 'sqlite_open';
			}

			// Connect to the MySQL server 
			$this->conn = $func($this->database,0666,$sqliteerror);
			if (!$this->conn) {
				return false;
			}
			return true;
		}

		function close()
		{
			return (@sqlite_close($this->conn));
		}

		function error()
		{
			return (sqlite_error_string(sqlite_last_error()));
		}

		function query($sql = '')
		{
			$this->result = sqlite_query($sql, $this->conn);
			return ($this->result != false);
		}

		function affectedRows()
		{
			return (@sqlite_changes($this->conn));
		}

		function numRows()
		{
			return (@sqlite_num_rows($this->result));
		}
		function fieldName($field)
		{
		   return (@sqlite_field_name($this->result,$field));
		}
		function insertID()
		{
			return (@sqlite_last_insert_rowid($this->conn));
		}
		
		function fetchObject()
		{
			$object=new stdClass;
			$tmp_arr=sqlite_fetch_array($this->result,SQLITE_NUM);
			if($tmp_arr!=false)
			{
				$i=0;
				foreach($tmp_arr as $value)
				{
					$fieldName=sqlite_field_name($this->result,$i);
					$object->$fieldName=$value;
					$i++;
				}
			}
			else
				return false;
			return $object;
		}

		function fetchArray()
		{
			return (@sqlite_fetch_array($this->result));
		}

		function fetchAssoc()
		{
			return (@sqlite_fetch_array($this->result,SQLITE_ASSOC));
		}

	}
?>
