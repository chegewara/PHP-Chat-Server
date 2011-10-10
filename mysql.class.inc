<?php

     /*
     ###############################################
     ####                                       ####
     ####    Author : Harish Chauhan            ####
     ####    Date   : 29 Sep,2004               ####
     ####    Updated: 21 Apr,2007               ####
     ####                                       ####
     ###############################################
     */

     class DB
     {
        ///Declaration of variables
        
	var $host = '';
	var $user = '';
	var $password = '';
	var $database = '';
	var $persistent = false;
        
        var $conn=NULL;
        var $result=false;
        var $fields;
        var $check_fields;
        var $tbname;
        var $addNewFlag=false;
        ///End

        function DB($host="",$user="",$password="",$dbname="",$open=false)
        {
         if($host!="")
            $this->host=$host;
         if($user!="")
            $this->user=$user;
         if($password!="")
            $this->password=$password;
         if($dbname!="")
            $this->database=$dbname;

         if($open)
           $this->open();
        }
        function open($host="",$user="",$password="",$dbname="") //
        {
         if($host!="")
            $this->host=$host;
         if($user!="")
            $this->user=$user;
         if($password!="")
            $this->password=$password;
         if($dbname!="")
            $this->database=$dbname;

         if($this->connect()===false) return false;
         if($this->select_db()===false) return false;
		 return $this->conn;
        }
        function set_host($host,$user,$password,$dbname)
        {
         $this->host=$host;
         $this->user=$user;
         $this->password=$password;
         $this->database=$dbname;
        }
        function affectedRows() //-- Get number of affected rows in previous operation
        {
         return @mysql_affected_rows($this->conn);
        }
        function close()//Close a connection to a  Server
        {
         return @mysql_close($this->conn);
        }
        function connect() //Open a connection to a Server
        {
          if(is_resource($this->conn))
			return true;
		  // Choose the appropriate connect function
          if ($this->persist)
	          $this->conn = @mysql_pconnect($this->host, $this->user, $this->password);
          else
	          $this->conn = @mysql_connect($this->host, $this->user, $this->password);

          // Connect to the database server
          if(!is_resource($this->conn))
             return false;
		  else
			  return true;
              
        }
        function select_db($dbname="") //Select a databse
        {
          if($dbname=="")
             $dbname=$this->database;
			return  @mysql_select_db($dbname,$this->conn);
        }
        function create_db($dbname="") //Create a database
        {
          if($dbname=="")
             $dbname=$this->database;
          return $this->query("CREATE DATABASE ".$dbname);
        }
        function drop_db($dbname="") //Drop a database
        {
          if($dbname=="")
             $dbname=$this->database;
          return $this->query("DROP DATABASE ".$dbname);
        }
        function error() //Get last error
        {
            return (mysql_error());
        }
        function errorno() //Get error number
        {
            return mysql_errno();
        }
        function query($sql = '') //Execute the sql query
        {
            $this->result = @mysql_query($sql, $this->conn);
            return $this->result;
        }
        function numRows($result=null) //Return number of rows in selected table
        {
        	if(!is_resource($result))
        		$result = $this->result;
            return (@mysql_num_rows($result));
        }
    	function fieldName($field, $result=null)
        {
        	if(!is_resource($result))
        		$result = $this->result;
           return (@mysql_field_name($result,$field));
        }
    	function insertID()
        {
            return (@mysql_insert_id($this->conn));
        }

        function data_seek($arg1,$row=0) ///Move internal result pointer
        {
        	if(is_resource($arg1))
        		$result = $arg1;
        	else
        		$result = $this->result;
        	
        	if(!is_resource($arg1) && !is_null($arg1))
        		$row = $arg1;

        	return mysql_data_seek($result,$row);
        }

        function fetchRow($result=null)
        {
        	if(!is_resource($result))
        		$result = $this->result;
            return (@mysql_fetch_row($result));
        }
        
        function fetchObject($result=null)
        {
        	if(!is_resource($result))
        		$result = $this->result;
            return (@mysql_fetch_object($result));
        }
        function fetchArray($arg1=null,$mode=MYSQL_BOTH)
        {
        	if(is_resource($arg1))
        		$result = $arg1;
        	else
        		$result = $this->result;
        	
        	if(!is_resource($arg1) && !is_null($arg1))
        		$mode = $arg1;
        	
            return (@mysql_fetch_array($result,$mode));
        }
        function fetchAssoc($result=null)
        {
        	if(!is_resource($result))
        		$result = $this->result;
            return (@mysql_fetch_assoc($result));
        }
        function freeResult($result=null)
        {
        	if(!is_resource($result))
        		$result = $this->result;
            return (@mysql_free_result($result));
        }
		function getSingleResult($sql)
		{
			$result = $this->query($sql);
			$row = $this->fetchArray($result,MYSQL_NUM);
			$return=$row[0];
			return $return;
		}
		
        function addNew($table_name)
        {
           $this->fields=array();
           $this->addNewFlag=true;
           $this->tbname=$table_name;
        }
        
        function edit($table_name)
        {
           $this->fields=array();
           $this->check_fields=array();
           $this->addNewFlag=false;
           $this->tbname=$table_name;
        }
        
        function update()
        {
         foreach($this->fields as $field_name=>$value)
		 {
           if($value=='--DATE--')
			   $qry.=$field_name."=now(),";
           else if(strtolower(trim($value))=='now()')
			   $qry.=$field_name."=now(),";
		   else
			   $qry.=$field_name."='".$value."',";
		 }
         $qry=substr($qry,0,strlen($qry)-1);

          if($this->addNewFlag)
            $qry="INSERT INTO ".$this->tbname." SET ".$qry;
          else
          {
           $qry="UPDATE ".$this->tbname." SET ".$qry;
           if(count($this->check_fields)>0 && is_array($this->check_fields))
           {
               $qry.=" WHERE ";
               foreach($this->check_fields as $field_name=>$value)
                   $qry.=$field_name."='".$value."' AND ";
               $qry=substr($qry,0,strlen($qry)-5);
           }
           else if(!empty($this->check_fields))
           {
               $qry.=" WHERE ".$this->check_fields." ";
           }
           
          }
         return $this->query($qry);
        }		
     }
?>
