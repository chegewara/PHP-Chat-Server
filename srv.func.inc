<?php
// Strip some chars from a string
function strip($string) {
	$string=trim($string);
	$string=mysql_real_escape_string($string);
	$string=str_replace("\n","",$string);
	$string=str_replace("\r","",$string);
	return $string;
}

// f(ucking) u(ncrackable) e(ncryption)
function fue($string, $times=NULL) {
	global $conf, $salt;

	$string=strip($string);
	if ($times==NULL) $times=$conf['fue'];
	// Execute the encryptions as many times as the user wants
	for($x=$times;$x>0;$x--) {
		$string=base64_encode($string);
		$string=md5($string);
		$string=sha1($string);
		$string=hash("sha256", $string.$salt);
		$string=hash("sha512", $string.$salt);
	}
	// Finaly, when done, return the value
	return $string;
}

// Notification, Server Side (SS)
function outp($msg,$type) {
	global $sock;

	if ($type=="i")
		$msg="[i] ".$msg;
	elseif ($type=="!")
		$msg="[!] ".$msg;
	elseif ($type=="x")
		$msg="[x] ".$msg;
	elseif ($type=="e") {
		$msg="[x] ".$msg;
		$exit=1;
	}

	echo $msg."\n";

	if ($exit==1) {
		socket_close($sock);
		unset($sock);
		exit(1);
	}
}

// Login
function check($user,$pass,$socket,$mode="login") {
	global $db, $usr, $i;

	$user1=fue($user);
	$pass=fue($pass);

	if ($mode=="login") {
		$xuery="SELECT * FROM users WHERE user='".$user1."' AND pass='".$pass."' ;";
		$check=$db->query($xuery);
		if (!$db->numRows($check)) {
			outp("Wrong login attempt for user \"".$user."\" from user \"".$usr[$i]["name"]."\"@socket ".$i."!",'x');
			socket_close($socket);
			unset($socket);
			return "ERR";
		}else{
			outp("User ".strip($user)." logged in",'i');
			return $user;
		}
	}elseif ($mode=="pass" && $user!=NULL && $pass!=NULL) {
		$check=$db->query("SELECT * FROM users WHERE user='".$user."' AND pass='".$pass."' ;");
		if ($db->numRows($check)==0) return "ERR";
		else return "OK";
	}else return "ERR";
}

function register($user,$pass,$email){
	global $conf, $db;

	if ($conf['active']==3){
		$sql="INSERT INTO users (user, pass, email, active) VALUES('".$user."', '".$pass."', '".$email."', true)";
		$db->query($sql);
		return "Thank you ".$user.", you can now login";
	}elseif ($conf['active']==0) return "Sorry, but you can't register";
	else{
		$sql="INSERT INTO users (user, pass, email, active) VALUES('".$user."', '".$pass."', false)";
		$db->query($sql);
		if ($conf['active']==1) return "You have been successfuly registered,\nbut to be able to login, the admin must activate your account";
		elseif ($conf['active']==2) return "You have been successfuly registered,\nbut to be able to login, you must activate your account by the e-mail you have been sent";
	}
}

// Change login data
function newdt($user,$pass,$nuser,$npass,$mode) {
	global $db;
	if ($user==NULL || $pass==NULL) return "ERR1";
	$user=fue($user);
	$pass=fue($pass);

	$ssql="UPDATE users SET";
	$esql="WHERE user='".$user."' AND pass='".$pass."' ;";

	//	User Panel	//
	if ($mode=="pass"){
		if ($npass==NULL) { $err=1; break; }
		$npass=fue($npass);
		$db->query($ssql." pass='".$npass."' ".$esql) or $err=1;
	}elseif ($mode=="user"){
		if ($nuser==NULL) { $err=1; break; }
		$nuser=fue($nuser);
		$db->query($ssql." user='".$nuser."' ".$esql) or $err=1;

	//	Admin Panel	//
	}elseif ($mode=="amake"){
		if ($npass==NULL || $nuser==NULL) { $err=1; break; }
		$nuser=fue($nuser);
		$npass=fue($npass);
		$db->query("INSERT INTO users (user,pass) VALUES('".$nuser."','".$npass."');") or $err=1;
	}elseif ($mode=="apass"){
		if ($user==NULL || $npass==NULL) { $err=1; break; }
		$npass=fue($npass);
		$db->query($ssql." pass='".$npass."' WHERE user='".$user."'") or $err=1;
	}elseif ($mode=="auser"){
		if ($user==NULL || $nuser==NULL) { $err=1; break; }
		$nuser=fue($nuser);
		$db->query($ssql." user='".$nuser."' WHERE user='".$user."'") or $err=1;
	}elseif ($mode=="adel"){
		if ($user==NULL) { $err=1; break; }
		$db->query("DELETE FROM users WHERE user='".$user."'");
	}elseif ($mode=="appr"){
	/*}else{
		$nuser=fue($nuser);
		$npass=fue($npass);
		$db->query($ssql." user='".$nuser."', pass='".$npass."' ".$esql) or $err=1; /*/
	}

	if ($err==1) return "ERR";
	else return "OK";
}
 
// Encrypt the text and send to socket
function socket_cwrite($socket,$string) {
	global $key, $conf;
	if ($conf["enc"]==true) {
		$string=base64_encode(mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $key, $string, MCRYPT_MODE_CBC, $key));
		$string=strrev($string);
	}
	socket_write($socket,$string);
}

// Read from socket and decrypt the text
function socket_cread($socket,$leng=NULL) {
	global $key, $conf;
	if ($leng==NULL) $leng=$conf['buff'];
	$string=socket_read($socket,$leng);
	if ($conf["enc"]==true) {
		$string=strrev($string);
		$string=rtrim(mcrypt_decrypt(MCRYPT_RIJNDAEL_256,$key,base64_decode($string),MCRYPT_MODE_CBC, $key),"\0");
	}
	return $string;
}

// Close socket properly
function socket_cclose($num, $socket) {
	socket_close($socket);
	unset($socket);
	return $num-1;
}

// Send PM
function PM($string,$user) {
	global $conf, $usr, $client;
	$opt=str_getopts($string,NULL,"PM");
	$opt["user"]=strip($opt["user"]);

	for ($x=0;$x<$conf['max'];$x++) {
		if ($usr[$x]==$opt["user"]) break;
	}
	if ($usr[$x]==$opt["user"] && $opt["user"]!=$usr[$user]) {
		socket_cwrite($client[$x]['sock'],"\n\t".$usr[$user]."->You: ".$opt["msg"]."\n");
		socket_cwrite($client[$x]['sock'],$conf["prompt"]);
		socket_cwrite($client[$user]['sock'],"\tYou->".$opt["user"].": ".$opt["msg"]."\n");
	}elseif ($opt["user"]==$usr[$user]) socket_cwrite($client[$user]['sock'],"You can't send PM to yourself!");
	else return "ERR";
}

// Check for double login
function doubleu($user) {
	global $usr, $conf, $client;
	for ($x=0;$x<$conf['max'];$x++) {
		if ($usr[$user]==$usr[$x] && $x!=$user){
			$err=1;
			break;
		}
	}
	if($err==1) return "ERR";
	else return "OK";
}

// Change a bool variable
function change($bool) {
	if ($bool==false) return true;
	elseif ($bool==true) return false;
	else return $bool;
}

// Separate a string in spaces
function str_getopts($string,$options,$mode="opts") {
	if ($mode=="opts") {
		for ($x=0;$x<$options;$x++) {
			sscanf($string,"%s",$opt[$x]);
			$string=str_replace($opt[$x]." ","",$string);
			unset($buff);
		}
		$opt["string"]=$string;
	}elseif ($mode=="PM") sscanf($string,"@%s %s",$opt["user"],$opt["msg"]);
	return $opt;
}
?>
