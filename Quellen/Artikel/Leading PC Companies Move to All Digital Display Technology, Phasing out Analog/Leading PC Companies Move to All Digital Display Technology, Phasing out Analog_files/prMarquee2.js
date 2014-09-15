$j(document).ready(function() {
    if(typeof documentID != "undefined"){
      var prMarqueeImgBase = "http://www.intel.com/newsroom/kits";

      switch(documentID) {

         case 1384:  // Intel Cloud 2015
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/cloud2015/images/nrJ5-mrq-cloud2015-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/cloud2015/images/nrJ5-mrq-cloud2015-txt.png" border="0"/>');
            break;

         case 1405:  // Intel Education
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/education/images/nrJ5-mrq-education-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/education/images/nrJ5-mrq-education-txt.png" border="0"/>');
            break;

         case 1406:  // Intel® Atom™ Processor
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/atom2/images/nrJ5-mrq-atom-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/atom2/images/nrJ5-mrq-atom-txt.png" border="0"/>');
            break;

         case 1512:  // Intel Introduces Configurable Intel® Atom™-based Processor
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/atom/e600c/images/nrJ5-mrq-configAtom-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/atom/e600c/images/nrJ5-mrq-configAtom-txt.png" border="0"/>');
            break;

         case 1543:  // Laptop Buying Guide
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/buyersguide/images/nrJ5-mrq-laptop-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/buyersguide/images/nrJ5-mrq-laptop-txt.png" border="0"/>');
            break;

         case 1551:  // Intel SSD
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/ssd/images/nrJ5-mrq-ssd-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/ssd/images/nrJ5-mrq-ssd-txt.png" border="0"/>');
            break;

         case 1641:  // 2nd Generation Intel® Core™ Processors
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/core/2ndgen/images/nrJ5-mrq-2ndGenCoreFam-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/core/2ndgen/images/nrJ5-mrq-2ndGenCoreFam-txt.png" border="0"/>');
            break;

         case 1831:  // Mobile World Congress 2011
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/mobileworld/2011/images/nrJ5-mrq-mwc2011-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/mobileworld/2011/images/nrJ5-mrq-mwc2011-txt.png" border="0"/>');
            break;

         case 1903:  // 2nd Generation Intel® Core™ vPro™ Processor Family
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/core/2ndgenvpro/images/nrJ5-mrq-2ndGen_vPro-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/core/2ndgenvpro/images/nrJ5-mrq-2ndGen_vPro-txt.png" border="0"/>');
            break;

         case 1964:  // Intel Embedded
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/embedded/images/nrJ5-mrq-embedded-bg-v3.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/embedded/images/nrJ5-mrq-embedded-txt-v3.png" border="0"/>');
            break;

         case 2032:  // Intel 22nm 3-D Tri-Gate Transistor Technology

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/22nm/images/nrJ5-mrq-22nmTrigate-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/22nm/images/nrJ5-mrq-22nmTrigate-txt.png" border="0"/>');
              break;

         case 2061:  // Intel at Computex Taipei 2011

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/computex/2011/images/nrJ5-mrq-computex2011-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/computex/2011/images/nrJ5-mrq-computex2011-txt.png" border="0"/>');
              break;

         case 2104:  // Research@Intel 2011
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/research/2011/images/nrJ5-mrq-research2011-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/research/2011/images/nrJ5-mrq-research2011-txt.png" border="0"/>');
              break;

         case 2194:  // Intel in the Cloud
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/inthecloud/images/nrJ5-mrq-intelCloud-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/inthecloud/images/nrJ5-mrq-intelCloud-txt.png" border="0"/>');
              break;

         case 2383:  // 40th Anniversary of the Microprocessor
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/40thanniversary/images/nrJ5-mrq-40thAnnivr-bg.gif)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/40thanniversary/images/nrJ5-mrq-40thAnnivr-txt.png" border="0"/>');
              break;

         case 2411:  // Ultrabook™
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/ultrabook/images/nrJ5-mrq-ultrabook-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/ultrabook/images/nrJ5-mrq-ultrabook-txt.png" border="0"/>');
              break;

         case 2476:  // From Sand to Silicon - the Making of a Chip
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/chipmaking/images/nrJ5-mrq-sandSilicon-bg.gif)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/chipmaking/images/nrJ5-mrq-sandSilicon-txt.png" border="0"/>');
              break;

         case 2567:  // Intel at CES 2012
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/ces/2012/images/nrJ5-mrq-CES2012-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/ces/2012/images/nrJ5-mrq-CES2012-txt.png" border="0"/>');
              break;

         case 2642: // Next-Generation Platform For Communications Networks
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/nextgencomm/images/nrJ5-mrq-nextGenComm-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/nextgencomm/images/nrJ5-mrq-nextGenComm-txt.png" border="0"/>');
              break;

         case 2652:  // Mobile World Congress 2012
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/mobileworld/2012/images/nrJ5-mrq-mwc2012-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/mobileworld/2012/images/nrJ5-mrq-mwc2012-txt.png" border="0"/>');
              break;

         case 2677:  // Intel® Xeon® Processor E5-2600/1600 Product Families
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/xeon/e5/images/nrJ5-mrq-xeonE5-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/xeon/e5/images/nrJ5-mrq-xeonE5-txt.png" border="0"/>');
              break;

         case 2735:  // 3rd Generation Intel® Core™ Processors
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/core/3rdgen/images/nrJ5-mrq-3rdGenCoreProc-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/core/3rdgen/images/nrJ5-mrq-3rdGenCoreProc-txt.png" border="0"/>');
              break;

         case 2764:  // 3rd Generation Intel® Core™ vPro Processors
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/core/3rdgen/vpro/images/nrJ5-mrq-3rdGenCoreFam-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/core/3rdgen/vpro/images/nrJ5-mrq-3rdGenCoreFam-txt.png" border="0"/>');
            break;

         case 2825:  // Intel at Computex Taipei 2012
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/computex/2012/images/nrJ5-mrq-computex2012-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/computex/2012/images/nrJ5-mrq-computex2012-txt.png" border="0"/>');
            break;

         case 2849:  // Intel Corporate
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/corporate/images/nrJ5-mrq-IntelCorp-bg_v2.gif)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/corporate/images/nrJ5-mrq-IntelCorp-txt_v1.png" border="0"/>');
            break;

         case 2857:  // Research@Intel 2012
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/research/2012/images/nrJ5-mrq-research2012-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/research/2012/images/nrJ5-mrq-research2012-txt.png" border="0"/>');
            break;

         case 2873:  // Ultrabook™ Experience Event: New York
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/ultrabook/images/nrJ5-mrq-ultrabook-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/ultrabook/images/nrJ5-mrq-ultrabookNY-txt.png" border="0"/>');
              break;

         case 2925:  //Intel Developer Forum 2012 San Francisco
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/idf/2012_fall/images/nrJ5-mrq-IDF2012-bg-v1.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/idf/2012_fall/images/nrJ5-mrq-IDF2012-txt-v1.png" border="0"/>');
              break;

         case 3126:  //Intel® Xeon Phi™ Coprocessor 5110P/3000 Series
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/xeon/phi/images/nrJ5-mrq-XeonPhi-bg-v1.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/xeon/phi/images/nrJ5-mrq-XeonPhi-txt-v1.png" border="0"/>');
              break;

         case 3153:  //Intel at CES 2013
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/ces/2013/images/nrJ5-mrq-CES2013-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/ces/2013/images/nrJ5-mrq-CES2013-txt.png" border="0"/>');
              break;

         case 3237:  //National Retail Federation Expo 2013
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/nrf/2013/images/nrJ5-mrq-natRetailFedExpo2013-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/nrf/2013/images/nrJ5-mrq-natRetailFedExpo2013-txt.png" border="0"/>');
              break;

         case 3267:  // Mobile World Congress 2013
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/mobileworld/2013/images/nrJ5-mrq-mwc2013-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/mobileworld/2013/images/nrJ5-mrq-mwc2013-txt.png" border="0"/>');
              break;

         case 3665:  // PC Refresh
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/pcrefresh/images/nrJ5-mrq-pcrefresh-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/pcrefresh/images/nrJ5-mrq-pcrefresh-txt.png" border="0"/>');
              break;

         case 3670:  // Intel Developer Forum (IDF) Beijing 2013
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/idf/2013_spring/images/nrJ5-mrq-IDF2013-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/idf/2013_spring/images/nrJ5-mrq-IDF2013-txt.png" border="0"/>');
              break;

         case 3743:  // 4th Generation Intel® Core™ Processors
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/core/4thgen/images/nrJ5-mrq-4th-gen-core-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/core/4thgen/images/nrJ5-mrq-4th-gen-core-txt.png" border="0"/>');
            break;

         case 3885:  // Intel at Computex Taipei 2013
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/computex/2013/images/nrJ5-mrq-computex2013-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/computex/2013/images/nrJ5-mrq-computex2013-txt.png" border="0"/>');
            break;

         case 3993:  // Research@Intel 2013
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/research/2013/images/nrJ5-mrq-research2013-bg.png)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/research/2013/images/nrJ5-mrq-research2013-txt.png" border="0"/>');
            break;

         case 4181:  //Intel Developer Forum 2013 San Francisco
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/idf/2013_fall/images/nrJ5-mrq-IDF2013SF-bg.gif)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/idf/2013_fall/images/nrJ5-mrq-IDF2013SF-txt.png" border="0"/>');
              break;

         case 4579:  //Holiday 2013: The Best Devices Have Intel Inside®
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/pcrefresh/holiday2013/images/nr-mrq-candy-bg.jpg)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/pcrefresh/holiday2013/images/nr-mrq-candy-txt.png" border="0"/>');
              break;

         case 4620:  //Intel at CES 2014
            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/ces/2014/images/nrJ5-mrq-li-CES-2014-bg.jpg)');
            $j("#j-header > a").html('<img src="'+prMarqueeImgBase+'/ces/2014/images/nrJ5-mrq-li-CES-2014-txt.png" border="0"/>');
              break;

         case 4631:  //Intel Newsroom Image Search
            $j('#j-header-wrap').css('background', 'url(http://www.intel.com/newsroom/assets/images/nrJ5-mrq-li-imageSearch-bg.png)');
            $j("#j-header > a").html('<img src="http://www.intel.com/newsroom/assets/images/nrJ5-mrq-li-imageSearch-txt.png" border="0"/>');
              break;

         case 4834:  //Intel and Conflict-Free Technology

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/conflictfree/images/nr-mrq-li-conflict-free-bg.jpg)');
              break;

         case 4840:  //National Retail Federation Expo 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/nrf/2014/images/nrJ5-mrq-li-NRF-2014-bg.jpg)');

            break;

         case 4854:  //Intel® Cloud Technology Program

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/cloudprogram/images/nrJ5-mrq-li-cloudtech-bg.jpg)');

            break;

         case 4943:  //Big Data Intelligence Begins With Intel

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/bigdata/images/nr-mrq-li-bigdata-bg.jpg)');

            break;

         case 5059:  //Intel at Mobile World Congress 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/mobileworld/2014/images/nr-mrq-li-mwc2014-bg.jpg)');

            break;

         case 5060:  //Intel at ISSCC 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/isscc/2014/images/nr-mrq-li-isscc14-bg.jpg)');

            break;

         case 5073:  //Intel® Xeon® Processor E7 v2 Product Family

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/xeon/e7v2/images/nr-mrq-li-xeon-E7-v2-bg.jpg)');

            break;

         case 5126:  //Intel at Embedded World 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/embeddedworld/2014/images/nr-li-intel-embedded-world-2014.jpg)');

            break;

         case 5192:  //Empowering Tomorrow's Innovators (Education SuperKit)

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/education/images/nr-mrq-li-sts.gif)');

            break;

         case 5224:  //Intel and the Internet of Things

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/iot/images/nr-kit-iot-mrq-bg.gif)');

            break;

         case 5226:  //Desktop PCs with Intel Inside®

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/desktop/2014/images/nr-mrq-4thgen-core-k-bg.jpg)');

            break;

         case 5256:  //Intel Developer Forum - Shenzhen 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/idf/2014_spring/images/nr-mrq-idf2014-shenzen-bg.jpg)');

            break;

         case 5285:  //Intel Making it 'Clear'

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/intelclear/images/nr-kit-font-mrq.gif)');

            break;

         case 5409:  //Intel and Chrome OS Press Event

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/chromebook/images/nr-mrq-li-chrome-proc-bg.jpg)');

            break;

         case 5412:  //Intel at Computex Taipei 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/computex/2014/images/nr-mrq-li-computex-2014-bg.jpg)');

            break;

         case 5413:  //Intel Enables Makers and Wearable Creators

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/wearables/images/nr-mrq-li-wearables-maker-bg.jpg)');

            break;

         case 5470:  //Intel Mobile and Communications

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/atom/comms/images/nr-mrq-li-mobile-and-comms-bg.jpg)');

            break;

         case 5481:  //Intel Future Showcase 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/futureshowcase/2014/images/nr-mrq-li-future-showcase-2014-bg.jpg)');

            break;

         case 5482:  //Intel® Pocket Avatars

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/pocketavatar/images/nr-mrq-li-pocket-avatar-bg.jpg)');

            break;

         case 5626:  //Intel at IFA 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/ifa/2014/images/nr-mrq-li-ifa-2014-bg.jpg)');

            break;

         case 5628:  //Software and Service for a Mobile World

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/atom/software/images/nr-mrq-li-mblsw-v1.jpg)');

            break;

         case 5677:  //Intel's Newest Microarchitecture and 14nm Manufacturing Process

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/14nm/images/nr-mrq-li-14nm-microarchitecture-bg.jpg)');

            break;

         case 5726:  //SMS Audio BioSport Headphones Powered by Intel

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/wearables/fitness/images/nr-mrq-li-biosport-headphones-bg.jpg)');

            break;

         case 5727:  //Intel Developer Forum San Francisco 2014

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/idf/2014_fall/images/nr-mrq-li-idf2014-sf-bg.jpg)');

            break;

         case 5761:  //Intel® Xeon® Processor E5-2600/1600 v3 Product Family

            $j('#j-header-wrap').css('background', 'url('+prMarqueeImgBase+'/xeon/e5v3/images/nr-mrq-li-e5-2600-v3-bg.jpg)');

            break;

                }
    }

  });
