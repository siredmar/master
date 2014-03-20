<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="6.3">
<drawing>
<settings>
<setting alwaysvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="SchlegelAr_Master">
<packages>
<package name="FRAME">
</package>
<package name="KLDX-0202-A-LT">
<wire x1="2.6" y1="-1.66" x2="2.6" y2="1.64" width="0" layer="46"/>
<wire x1="2.6" y1="1.64" x2="3.6" y2="1.64" width="0" layer="46"/>
<wire x1="3.6" y1="1.64" x2="3.6" y2="-1.66" width="0" layer="46"/>
<wire x1="3.6" y1="-1.66" x2="2.6" y2="-1.66" width="0" layer="46"/>
<wire x1="1.4" y1="-5.31" x2="1.4" y2="-4.31" width="0" layer="46"/>
<wire x1="1.4" y1="-4.31" x2="-1.4" y2="-4.31" width="0" layer="46"/>
<wire x1="-1.4" y1="-4.31" x2="-1.4" y2="-5.31" width="0" layer="46"/>
<wire x1="-1.4" y1="-5.31" x2="1.4" y2="-5.31" width="0" layer="46"/>
<wire x1="-3.5" y1="-1.41" x2="-3.5" y2="1.39" width="0" layer="46"/>
<wire x1="-3.5" y1="1.39" x2="-2.5" y2="1.39" width="0" layer="46"/>
<wire x1="-2.5" y1="1.39" x2="-2.5" y2="-1.41" width="0" layer="46"/>
<wire x1="-2.5" y1="-1.41" x2="-3.5" y2="-1.41" width="0" layer="46"/>
<wire x1="-10.4" y1="4.29" x2="-10.4" y2="-4.31" width="0.2032" layer="21"/>
<wire x1="-10.4" y1="-4.31" x2="3.9" y2="-4.31" width="0.2032" layer="51"/>
<wire x1="3.9" y1="-4.31" x2="3.9" y2="4.29" width="0.2032" layer="51"/>
<wire x1="3.9" y1="4.29" x2="-10.4" y2="4.29" width="0.2032" layer="21"/>
<wire x1="3.9" y1="2.69" x2="3.9" y2="4.29" width="0.2032" layer="21"/>
<wire x1="3.9" y1="-4.31" x2="3.9" y2="-2.71" width="0.2032" layer="21"/>
<wire x1="-10.275" y1="2.99" x2="-3" y2="2.99" width="0.2032" layer="51" style="shortdash"/>
<wire x1="-10.3" y1="-3.01" x2="-3" y2="-3.01" width="0.2032" layer="51" style="shortdash"/>
<wire x1="-3" y1="-3.01" x2="-3" y2="2.99" width="0.2032" layer="51" style="shortdash"/>
<wire x1="-9" y1="0.89" x2="-4.5" y2="0.89" width="0.2032" layer="51" style="shortdash"/>
<wire x1="-9" y1="-0.91" x2="-4.5" y2="-0.91" width="0.2032" layer="51" style="shortdash"/>
<wire x1="-9" y1="0.89" x2="-9" y2="-0.91" width="0.2032" layer="51" curve="166.57811"/>
<wire x1="-10.4" y1="-4.31" x2="-2.45" y2="-4.31" width="0.2032" layer="21"/>
<wire x1="2.3" y1="-4.31" x2="3.9" y2="-4.31" width="0.2032" layer="21"/>
<pad name="1" x="3.1" y="-0.01" drill="1" diameter="2" shape="long" rot="R90"/>
<pad name="3" x="-3" y="-0.01" drill="1" diameter="2" shape="long" rot="R90"/>
<pad name="2" x="0" y="-4.81" drill="1" diameter="2" shape="long"/>
<text x="-10.16" y="-6.36" size="1.27" layer="25">&gt;NAME</text>
<text x="-10.16" y="5.07" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="C_POL_4X4MM">
<wire x1="1.25" y1="-2.15" x2="-2.15" y2="-2.15" width="0.2032" layer="21"/>
<wire x1="-2.15" y1="-2.15" x2="-2.15" y2="-1.1" width="0.2032" layer="21"/>
<wire x1="-2.15" y1="-1.1" x2="-2.15" y2="1.1" width="0.2032" layer="51"/>
<wire x1="-2.15" y1="1.1" x2="-2.15" y2="2.15" width="0.2032" layer="21"/>
<wire x1="-2.15" y1="2.15" x2="1.25" y2="2.15" width="0.2032" layer="21"/>
<wire x1="2.15" y1="1.25" x2="2.15" y2="1.1" width="0.2032" layer="21"/>
<wire x1="2.15" y1="1.1" x2="2.15" y2="-1.05" width="0.2032" layer="51"/>
<wire x1="2.15" y1="-1.05" x2="2.15" y2="-1.25" width="0.2032" layer="21"/>
<wire x1="2.15" y1="-1.25" x2="1.25" y2="-2.15" width="0.2032" layer="21"/>
<wire x1="1.25" y1="2.15" x2="2.15" y2="1.25" width="0.2032" layer="21"/>
<wire x1="-1.65" y1="-0.95" x2="1.65" y2="-0.95" width="0.2032" layer="21" curve="120.136976"/>
<wire x1="-1.65" y1="-0.95" x2="-1.65" y2="0.95" width="0.2032" layer="51" curve="-59.863024"/>
<wire x1="-1.65" y1="0.95" x2="1.65" y2="0.95" width="0.2032" layer="21" curve="-120.136976"/>
<wire x1="1.65" y1="-0.95" x2="1.65" y2="0.95" width="0.2032" layer="51" curve="59.863024"/>
<smd name="+" x="1.8" y="0" dx="2.6" dy="1.6" layer="1"/>
<smd name="-" x="-1.8" y="0" dx="2.6" dy="1.6" layer="1"/>
<text x="-2.24" y="2.48" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.275" y="-3.735" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="C_POL_6X6MM">
<wire x1="2.4" y1="-3.23" x2="-3.3" y2="-3.23" width="0.2032" layer="21"/>
<wire x1="-3.3" y1="-3.23" x2="-3.3" y2="-1.03" width="0.2032" layer="21"/>
<wire x1="-3.3" y1="-1.03" x2="-3.3" y2="1.17" width="0.2032" layer="51"/>
<wire x1="-3.3" y1="1.17" x2="-3.3" y2="3.37" width="0.2032" layer="21"/>
<wire x1="-3.3" y1="3.37" x2="2.4" y2="3.37" width="0.2032" layer="21"/>
<wire x1="3.3" y1="2.47" x2="3.3" y2="1.17" width="0.2032" layer="21"/>
<wire x1="3.3" y1="1.17" x2="3.3" y2="-1.03" width="0.2032" layer="51"/>
<wire x1="3.3" y1="-1.03" x2="3.3" y2="-2.33" width="0.2032" layer="21"/>
<wire x1="3.3" y1="-2.33" x2="2.4" y2="-3.23" width="0.2032" layer="21"/>
<wire x1="2.4" y1="3.37" x2="3.3" y2="2.47" width="0.2032" layer="21"/>
<wire x1="-2.95" y1="-0.83" x2="2.95" y2="-0.88" width="0.2032" layer="21" curve="145.181395"/>
<wire x1="-2.95" y1="-0.83" x2="-2.95" y2="1.02" width="0.2032" layer="51" curve="-34.818605"/>
<wire x1="-2.95" y1="1.02" x2="2.95" y2="0.97" width="0.2032" layer="21" curve="-145.181395"/>
<wire x1="2.95" y1="-0.88" x2="2.95" y2="0.97" width="0.2032" layer="51" curve="34.818605"/>
<smd name="+" x="2.7" y="0.07" dx="3.5" dy="1.6" layer="1"/>
<smd name="-" x="-2.7" y="0.07" dx="3.5" dy="1.6" layer="1"/>
<text x="-3.39" y="3.7" size="1.27" layer="25">&gt;NAME</text>
<text x="-3.425" y="-4.815" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="C_POL_10X10MM">
<wire x1="-5.8" y1="5.15" x2="4.35" y2="5.15" width="0.2032" layer="21"/>
<wire x1="5.8" y1="3.7" x2="5.8" y2="1.45" width="0.2032" layer="21"/>
<wire x1="5.8" y1="1.45" x2="5.8" y2="-1.45" width="0.2032" layer="51"/>
<wire x1="5.8" y1="-1.45" x2="5.8" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="4.35" y1="-5.15" x2="-5.8" y2="-5.15" width="0.2032" layer="21"/>
<wire x1="-5.8" y1="-5.15" x2="-5.8" y2="-1.45" width="0.2032" layer="21"/>
<wire x1="-5.8" y1="-1.45" x2="-5.8" y2="1.45" width="0.2032" layer="51"/>
<wire x1="-5.8" y1="1.45" x2="-5.8" y2="5.15" width="0.2032" layer="21"/>
<wire x1="5.8" y1="3.7" x2="4.35" y2="5.15" width="0.2032" layer="21"/>
<wire x1="5.8" y1="-3.7" x2="4.35" y2="-5.15" width="0.2032" layer="21"/>
<wire x1="-4.7" y1="-1.4" x2="4.7" y2="-1.4" width="0.2032" layer="21" curve="146.825323"/>
<wire x1="4.7" y1="-1.4" x2="4.7" y2="1.4" width="0.2032" layer="51" curve="33.174677"/>
<wire x1="-4.7" y1="1.4" x2="4.7" y2="1.4" width="0.2032" layer="21" curve="-146.825323"/>
<wire x1="-4.7" y1="-1.4" x2="-4.7" y2="1.4" width="0.2032" layer="51" curve="-33.174677"/>
<smd name="+" x="4.15" y="0" dx="4.3" dy="2.5" layer="1"/>
<smd name="-" x="-4.15" y="0" dx="4.3" dy="2.5" layer="1"/>
<text x="-2.74" y="1.88" size="1.27" layer="25">&gt;NAME</text>
<text x="-3.025" y="-2.935" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="MBS">
<wire x1="-2.325" y1="1.975" x2="2.325" y2="1.975" width="0.254" layer="21"/>
<wire x1="2.325" y1="1.975" x2="2.325" y2="-1.975" width="0.254" layer="21"/>
<wire x1="2.325" y1="-1.975" x2="-2.325" y2="-1.975" width="0.254" layer="21"/>
<wire x1="-2.325" y1="-1.975" x2="-2.325" y2="1.975" width="0.254" layer="21"/>
<wire x1="-1.55" y1="-1.175" x2="-1" y2="-1.175" width="0.1016" layer="21"/>
<wire x1="-1.275" y1="-1.425" x2="-1.275" y2="-0.9" width="0.1016" layer="21"/>
<wire x1="0.99" y1="-1.175" x2="1.54" y2="-1.175" width="0.1016" layer="21"/>
<wire x1="-1.25" y1="1.25" x2="-1.625" y2="1.25" width="0.1016" layer="21" curve="172.37185"/>
<wire x1="-0.875" y1="1.25" x2="-1.25" y2="1.25" width="0.1016" layer="21" curve="-172.37185"/>
<wire x1="1.25" y1="1.25" x2="1.625" y2="1.25" width="0.1016" layer="21" curve="172.37185"/>
<wire x1="0.875" y1="1.25" x2="1.25" y2="1.25" width="0.1016" layer="21" curve="-172.37185"/>
<smd name="+" x="-1.27" y="-3" dx="1" dy="1.3" layer="1"/>
<smd name="-" x="1.27" y="-3" dx="1" dy="1.3" layer="1"/>
<smd name="AC2" x="1.27" y="3" dx="1" dy="1.3" layer="1" rot="R180"/>
<smd name="AC1" x="-1.27" y="3" dx="1" dy="1.3" layer="1" rot="R180"/>
<text x="-2.885" y="-4.175" size="1.778" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="5.155" y="-4.425" size="1.778" layer="27" ratio="10" rot="R90">&gt;VALUE</text>
<rectangle x1="-1.65" y1="2.075" x2="-0.875" y2="3.505" layer="51" rot="R180"/>
<rectangle x1="0.89" y1="2.075" x2="1.665" y2="3.505" layer="51" rot="R180"/>
<rectangle x1="0.875" y1="-3.505" x2="1.65" y2="-2.075" layer="51"/>
<rectangle x1="-1.665" y1="-3.505" x2="-0.89" y2="-2.075" layer="51"/>
</package>
<package name="SOT223">
<wire x1="3.2766" y1="1.778" x2="3.2766" y2="-1.778" width="0.2032" layer="21"/>
<wire x1="3.2766" y1="-1.778" x2="-3.2766" y2="-1.778" width="0.2032" layer="21"/>
<wire x1="-3.2766" y1="-1.778" x2="-3.2766" y2="1.778" width="0.2032" layer="21"/>
<wire x1="-3.2766" y1="1.778" x2="3.2766" y2="1.778" width="0.2032" layer="21"/>
<smd name="1" x="-2.3114" y="-3.0988" dx="1.2192" dy="2.2352" layer="1"/>
<smd name="2" x="0" y="-3.0988" dx="1.2192" dy="2.2352" layer="1"/>
<smd name="3" x="2.3114" y="-3.0988" dx="1.2192" dy="2.2352" layer="1"/>
<smd name="4" x="0" y="3.099" dx="3.6" dy="2.2" layer="1"/>
<text x="1.0208" y="-4.318" size="0.8128" layer="21" ratio="12">3</text>
<text x="1.905" y="2.54" size="0.8128" layer="21" ratio="12">4</text>
<text x="-3.4526" y="-4.318" size="0.8128" layer="21" ratio="12">1</text>
<text x="-1.2906" y="-4.3274" size="0.8128" layer="21" ratio="12">2</text>
<text x="-2.54" y="0.0508" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.54" y="-1.3208" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-1.6002" y1="1.8034" x2="1.6002" y2="3.6576" layer="51"/>
<rectangle x1="-0.4318" y1="-3.6576" x2="0.4318" y2="-1.8034" layer="51"/>
<rectangle x1="-2.7432" y1="-3.6576" x2="-1.8796" y2="-1.8034" layer="51"/>
<rectangle x1="1.8796" y1="-3.6576" x2="2.7432" y2="-1.8034" layer="51"/>
<rectangle x1="-1.6002" y1="1.8034" x2="1.6002" y2="3.6576" layer="51"/>
<rectangle x1="-0.4318" y1="-3.6576" x2="0.4318" y2="-1.8034" layer="51"/>
<rectangle x1="-2.7432" y1="-3.6576" x2="-1.8796" y2="-1.8034" layer="51"/>
<rectangle x1="1.8796" y1="-3.6576" x2="2.7432" y2="-1.8034" layer="51"/>
</package>
</packages>
<symbols>
<symbol name="FRAME_A3_LANDSCAPE">
<wire x1="288.29" y1="3.81" x2="342.265" y2="3.81" width="0.1016" layer="94"/>
<wire x1="342.265" y1="3.81" x2="373.38" y2="3.81" width="0.1016" layer="94"/>
<wire x1="373.38" y1="3.81" x2="383.54" y2="3.81" width="0.1016" layer="94"/>
<wire x1="383.54" y1="3.81" x2="383.54" y2="8.89" width="0.1016" layer="94"/>
<wire x1="383.54" y1="8.89" x2="383.54" y2="13.97" width="0.1016" layer="94"/>
<wire x1="383.54" y1="13.97" x2="383.54" y2="19.05" width="0.1016" layer="94"/>
<wire x1="383.54" y1="19.05" x2="383.54" y2="24.13" width="0.1016" layer="94"/>
<wire x1="288.29" y1="3.81" x2="288.29" y2="24.13" width="0.1016" layer="94"/>
<wire x1="288.29" y1="24.13" x2="342.265" y2="24.13" width="0.1016" layer="94"/>
<wire x1="342.265" y1="24.13" x2="383.54" y2="24.13" width="0.1016" layer="94"/>
<wire x1="373.38" y1="3.81" x2="373.38" y2="8.89" width="0.1016" layer="94"/>
<wire x1="373.38" y1="8.89" x2="383.54" y2="8.89" width="0.1016" layer="94"/>
<wire x1="373.38" y1="8.89" x2="342.265" y2="8.89" width="0.1016" layer="94"/>
<wire x1="342.265" y1="8.89" x2="342.265" y2="3.81" width="0.1016" layer="94"/>
<wire x1="342.265" y1="8.89" x2="342.265" y2="13.97" width="0.1016" layer="94"/>
<wire x1="342.265" y1="13.97" x2="383.54" y2="13.97" width="0.1016" layer="94"/>
<wire x1="342.265" y1="13.97" x2="342.265" y2="19.05" width="0.1016" layer="94"/>
<wire x1="342.265" y1="19.05" x2="383.54" y2="19.05" width="0.1016" layer="94"/>
<wire x1="342.265" y1="19.05" x2="342.265" y2="24.13" width="0.1016" layer="94"/>
<text x="344.17" y="15.24" size="2.54" layer="94" font="vector">&gt;DRAWING_NAME</text>
<text x="344.17" y="10.16" size="2.286" layer="94" font="vector">&gt;LAST_DATE_TIME</text>
<text x="357.505" y="5.08" size="2.54" layer="94" font="vector">&gt;SHEET</text>
<text x="343.916" y="4.953" size="2.54" layer="94" font="vector">Sheet:</text>
<frame x1="0" y1="0" x2="387.35" y2="260.35" columns="8" rows="5" layer="94"/>
</symbol>
<symbol name="SUP_CON">
<wire x1="2.54" y1="2.54" x2="-5.08" y2="2.54" width="0.1524" layer="94"/>
<wire x1="-5.08" y1="2.54" x2="-5.08" y2="1.27" width="0.1524" layer="94"/>
<wire x1="2.54" y1="0" x2="0" y2="0" width="0.1524" layer="94"/>
<wire x1="0" y1="0" x2="0" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="0" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="0" y1="-2.54" x2="-1.778" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="-1.778" y1="-2.54" x2="-2.54" y2="-1.016" width="0.1524" layer="94"/>
<wire x1="-2.54" y1="-1.016" x2="-3.302" y2="-2.54" width="0.1524" layer="94"/>
<text x="-5.08" y="3.81" size="1.778" layer="95">&gt;NAME</text>
<text x="-5.08" y="-6.35" size="1.778" layer="96">&gt;VALUE</text>
<rectangle x1="-5.842" y1="-2.54" x2="-4.318" y2="1.27" layer="94"/>
<pin name="1" x="5.08" y="2.54" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="2" x="5.08" y="0" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="3" x="5.08" y="-2.54" visible="pad" length="short" direction="pas" rot="R180"/>
<polygon width="0.1524" layer="94">
<vertex x="0" y="-2.54"/>
<vertex x="-0.508" y="-1.27"/>
<vertex x="0.508" y="-1.27"/>
</polygon>
</symbol>
<symbol name="C_POL">
<pin name="P$1" x="-5.08" y="0" visible="off" length="short" direction="pas"/>
<pin name="P$2" x="2.54" y="0" visible="off" length="short" direction="pas" rot="R180"/>
<wire x1="-2.54" y1="1.778" x2="-2.54" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-1.778" x2="-1.524" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-1.524" y1="-1.778" x2="-1.524" y2="1.778" width="0.254" layer="94"/>
<wire x1="-2.54" y1="1.778" x2="-1.524" y2="1.778" width="0.254" layer="94"/>
<wire x1="0" y1="-1.778" x2="0" y2="1.778" width="0.254" layer="94"/>
<wire x1="0" y1="1.778" x2="-0.254" y2="1.778" width="0.254" layer="94"/>
<wire x1="-0.254" y1="1.778" x2="-0.254" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-0.254" y1="1.778" x2="-0.508" y2="1.778" width="0.254" layer="94"/>
<wire x1="-0.508" y1="1.778" x2="-0.762" y2="1.778" width="0.254" layer="94"/>
<wire x1="-0.762" y1="1.778" x2="-0.762" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-0.762" y1="-1.778" x2="-0.508" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-0.508" y1="-1.778" x2="-0.254" y2="-1.778" width="0.254" layer="94"/>
<wire x1="0" y1="-1.778" x2="-0.254" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-0.508" y1="-1.778" x2="-0.508" y2="1.778" width="0.254" layer="94"/>
<wire x1="-1.016" y1="1.778" x2="-1.016" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-1.016" y1="-1.778" x2="-0.762" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-1.016" y1="1.778" x2="-0.762" y2="1.778" width="0.254" layer="94"/>
<wire x1="-3.556" y1="-0.762" x2="-3.556" y2="-1.778" width="0.254" layer="94"/>
<wire x1="-4.064" y1="-1.27" x2="-3.048" y2="-1.27" width="0.254" layer="94"/>
<text x="-2.794" y="2.286" size="1.27" layer="95">&gt;NAME</text>
<text x="-2.794" y="-3.548" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="RECTIFIER">
<wire x1="-1.905" y1="-3.175" x2="-4.064" y2="-2.794" width="0.254" layer="94"/>
<wire x1="-1.905" y1="-3.175" x2="-2.286" y2="-1.016" width="0.254" layer="94"/>
<wire x1="-2.7178" y1="-4.0386" x2="-0.9398" y2="-2.2606" width="0.254" layer="94"/>
<wire x1="-1.905" y1="3.175" x2="-4.064" y2="2.794" width="0.254" layer="94"/>
<wire x1="-1.905" y1="3.175" x2="-2.286" y2="1.016" width="0.254" layer="94"/>
<wire x1="-2.7178" y1="4.0386" x2="-1.0668" y2="2.3876" width="0.254" layer="94"/>
<wire x1="3.175" y1="1.905" x2="2.794" y2="4.064" width="0.254" layer="94"/>
<wire x1="3.175" y1="1.905" x2="1.016" y2="2.286" width="0.254" layer="94"/>
<wire x1="2.3622" y1="1.016" x2="4.1402" y2="2.794" width="0.254" layer="94"/>
<wire x1="3.175" y1="-1.905" x2="2.794" y2="-4.064" width="0.254" layer="94"/>
<wire x1="3.175" y1="-1.905" x2="1.016" y2="-2.286" width="0.254" layer="94"/>
<wire x1="2.3622" y1="-1.0668" x2="4.1402" y2="-2.8448" width="0.254" layer="94"/>
<wire x1="5.08" y1="0" x2="3.175" y2="-1.905" width="0.1524" layer="94"/>
<wire x1="0" y1="-5.08" x2="-1.905" y2="-3.175" width="0.1524" layer="94"/>
<wire x1="-3.2766" y1="-1.8034" x2="-5.08" y2="0" width="0.1524" layer="94"/>
<wire x1="-4.064" y1="-2.794" x2="-2.286" y2="-1.016" width="0.254" layer="94"/>
<wire x1="-5.08" y1="0" x2="-3.2766" y2="1.8034" width="0.1524" layer="94"/>
<wire x1="-4.064" y1="2.794" x2="-2.286" y2="1.016" width="0.254" layer="94"/>
<wire x1="-1.905" y1="3.175" x2="0" y2="5.08" width="0.1524" layer="94"/>
<wire x1="0" y1="5.08" x2="1.8034" y2="3.2766" width="0.1524" layer="94"/>
<wire x1="1.016" y1="2.286" x2="2.794" y2="4.064" width="0.254" layer="94"/>
<wire x1="3.175" y1="1.905" x2="5.08" y2="0" width="0.1524" layer="94"/>
<wire x1="0" y1="-5.08" x2="1.8034" y2="-3.2766" width="0.1524" layer="94"/>
<wire x1="1.016" y1="-2.286" x2="2.794" y2="-4.064" width="0.254" layer="94"/>
<text x="5.08" y="-5.08" size="1.778" layer="96">&gt;VALUE</text>
<text x="5.08" y="2.54" size="1.778" layer="95">&gt;NAME</text>
<pin name="AC1" x="0" y="5.08" visible="off" length="point" direction="pas" rot="R270"/>
<pin name="+" x="5.08" y="0" visible="off" length="point" direction="pas" rot="R180"/>
<pin name="AC2" x="0" y="-5.08" visible="off" length="point" direction="pas" rot="R90"/>
<pin name="-" x="-5.08" y="0" visible="off" length="point" direction="pas"/>
</symbol>
<symbol name="VCC_EXT">
<wire x1="1.27" y1="0.635" x2="0" y2="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="2.54" x2="-1.27" y2="0.635" width="0.254" layer="94"/>
<text x="-2.54" y="-2.54" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="VCC_EXT" x="0" y="0" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
<symbol name="GND">
<wire x1="-1.905" y1="-2.54" x2="1.905" y2="-2.54" width="0.254" layer="94"/>
<text x="-2.54" y="-5.08" size="1.778" layer="96">&gt;VALUE</text>
<pin name="GND" x="0" y="0" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
<symbol name="GND_EXT">
<wire x1="-1.905" y1="-2.54" x2="1.905" y2="-2.54" width="0.254" layer="94"/>
<text x="-2.54" y="-5.08" size="1.778" layer="96">&gt;VALUE</text>
<pin name="GND_EXT" x="0" y="0" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
<symbol name="VREG">
<wire x1="-5.08" y1="-5.08" x2="5.08" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="5.08" y1="-5.08" x2="5.08" y2="5.08" width="0.4064" layer="94"/>
<wire x1="5.08" y1="5.08" x2="-5.08" y2="5.08" width="0.4064" layer="94"/>
<wire x1="-5.08" y1="5.08" x2="-5.08" y2="-5.08" width="0.4064" layer="94"/>
<text x="2.54" y="-7.62" size="1.778" layer="95">&gt;NAME</text>
<text x="2.54" y="-10.16" size="1.778" layer="96">&gt;VALUE</text>
<text x="-4.572" y="-4.318" size="1.524" layer="95">ADJ/GND</text>
<text x="-4.445" y="1.905" size="1.524" layer="95">IN</text>
<text x="0.635" y="1.905" size="1.524" layer="95">OUT</text>
<pin name="IN" x="-7.62" y="2.54" visible="off" length="short" direction="in"/>
<pin name="GND" x="0" y="-7.62" visible="off" length="short" direction="in" rot="R90"/>
<pin name="OUT" x="7.62" y="2.54" visible="off" length="short" direction="pas" rot="R180"/>
<pin name="OUT1" x="7.62" y="0" visible="off" length="short" direction="pas" rot="R180"/>
<text x="0.635" y="-0.635" size="1.524" layer="95">OUT</text>
</symbol>
<symbol name="+3V3">
<wire x1="1.27" y1="0.635" x2="0" y2="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="2.54" x2="-1.27" y2="0.635" width="0.254" layer="94"/>
<text x="-2.54" y="-2.54" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="+3V3" x="0" y="0" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="FRAME_A3_LANDSCAPE">
<description>&lt;b&gt;
A3 Frame querformat

&lt;/b&gt;&lt;br&gt;&lt;br&gt;last modified: Armin Schlegel, 20.03.2014  &lt;br&gt;&lt;br&gt;</description>
<gates>
<gate name="G$1" symbol="FRAME_A3_LANDSCAPE" x="0" y="0"/>
</gates>
<devices>
<device name="" package="FRAME">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="KLDX-0202-A-LT" prefix="CON">
<description>&lt;b&gt;
KLDX-0202-A-LT

&lt;/b&gt;&lt;br&gt;&lt;br&gt;last modified: Armin Schlegel, 20.03.2014  &lt;br&gt;&lt;br&gt;

http://de.mouser.com/ProductDetail/Kycon/KLDX-0202-A-LT/?qs=%2fha2pyFadug6yAePtFHF%252bYgVUqD4usJO%252bzszO19jtQE%252bUyITMB7Piw%3d%3d
&lt;br&gt; &lt;br&gt;
http://www.mouser.com/ds/2/222/KLDX_0202_LT-221016.pdf</description>
<gates>
<gate name="G$1" symbol="SUP_CON" x="0" y="0"/>
</gates>
<devices>
<device name="" package="KLDX-0202-A-LT">
<connects>
<connect gate="G$1" pin="1" pad="3"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="C_POL" prefix="C" uservalue="yes">
<description>&lt;b&gt;
C Pol

&lt;/b&gt;&lt;br&gt;&lt;br&gt;last modified: Armin Schlegel, 20.03.2014  &lt;br&gt;&lt;br&gt;</description>
<gates>
<gate name="G$1" symbol="C_POL" x="0" y="0"/>
</gates>
<devices>
<device name="4X4MM" package="C_POL_4X4MM">
<connects>
<connect gate="G$1" pin="P$1" pad="+"/>
<connect gate="G$1" pin="P$2" pad="-"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="6X6MM" package="C_POL_6X6MM">
<connects>
<connect gate="G$1" pin="P$1" pad="+"/>
<connect gate="G$1" pin="P$2" pad="-"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="10X10MM" package="C_POL_10X10MM">
<connects>
<connect gate="G$1" pin="P$1" pad="+"/>
<connect gate="G$1" pin="P$2" pad="-"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="MBS10" prefix="IC">
<description>&lt;b&gt;
MBS10

&lt;/b&gt;&lt;br&gt;&lt;br&gt;last modified: Armin Schlegel, 20.03.2014  &lt;br&gt;&lt;br&gt;

http://de.mouser.com/ProductDetail/Taiwan-Semiconductor/MBS10/?qs=sGAEpiMZZMutXGli8Ay4kJ6cJI5FYvVSu5UYMaNpkuw%3d
&lt;br&gt; &lt;br&gt;
http://www.mouser.com/ds/2/395/MBS2%20SERIES_J12-248491.pdf</description>
<gates>
<gate name="G$1" symbol="RECTIFIER" x="0" y="0"/>
</gates>
<devices>
<device name="" package="MBS">
<connects>
<connect gate="G$1" pin="+" pad="+"/>
<connect gate="G$1" pin="-" pad="-"/>
<connect gate="G$1" pin="AC1" pad="AC1"/>
<connect gate="G$1" pin="AC2" pad="AC2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="VCC_EXT">
<gates>
<gate name="G$1" symbol="VCC_EXT" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="GND">
<gates>
<gate name="G$1" symbol="GND" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="GND_EXT">
<gates>
<gate name="G$1" symbol="GND_EXT" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="TLV1117-33CDCYR" prefix="IC">
<description>&lt;b&gt;
TLV1117-33CDCYR

&lt;/b&gt;&lt;br&gt;&lt;br&gt;last modified: Armin Schlegel, 20.03.2014  &lt;br&gt;&lt;br&gt;

http://de.mouser.com/ProductDetail/Texas-Instruments/TLV1117-33CDCYR/?qs=%2fha2pyFadugmKJNCeNNTQcrKFxP%2fkxEU2Vvep9wAQZWkNmX%252bPSB9X468j5xdkpmb
&lt;br&gt; &lt;br&gt;
http://www.ti.com/litv/slvs561k</description>
<gates>
<gate name="G$1" symbol="VREG" x="0" y="0"/>
</gates>
<devices>
<device name="" package="SOT223">
<connects>
<connect gate="G$1" pin="GND" pad="1"/>
<connect gate="G$1" pin="IN" pad="3"/>
<connect gate="G$1" pin="OUT" pad="2"/>
<connect gate="G$1" pin="OUT1" pad="4"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="+3V3">
<gates>
<gate name="G$1" symbol="+3V3" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="U$1" library="SchlegelAr_Master" deviceset="FRAME_A3_LANDSCAPE" device=""/>
<part name="U$2" library="SchlegelAr_Master" deviceset="FRAME_A3_LANDSCAPE" device=""/>
<part name="U$3" library="SchlegelAr_Master" deviceset="FRAME_A3_LANDSCAPE" device=""/>
<part name="U$4" library="SchlegelAr_Master" deviceset="FRAME_A3_LANDSCAPE" device=""/>
<part name="U$5" library="SchlegelAr_Master" deviceset="FRAME_A3_LANDSCAPE" device=""/>
<part name="CON1" library="SchlegelAr_Master" deviceset="KLDX-0202-A-LT" device=""/>
<part name="C1" library="SchlegelAr_Master" deviceset="C_POL" device="6X6MM" value="10u"/>
<part name="IC1" library="SchlegelAr_Master" deviceset="MBS10" device=""/>
<part name="U$7" library="SchlegelAr_Master" deviceset="VCC_EXT" device=""/>
<part name="U$6" library="SchlegelAr_Master" deviceset="VCC_EXT" device=""/>
<part name="U$8" library="SchlegelAr_Master" deviceset="GND" device=""/>
<part name="U$9" library="SchlegelAr_Master" deviceset="GND" device=""/>
<part name="U$10" library="SchlegelAr_Master" deviceset="GND_EXT" device=""/>
<part name="U$11" library="SchlegelAr_Master" deviceset="GND_EXT" device=""/>
<part name="IC2" library="SchlegelAr_Master" deviceset="TLV1117-33CDCYR" device=""/>
<part name="U$12" library="SchlegelAr_Master" deviceset="GND" device=""/>
<part name="C2" library="SchlegelAr_Master" deviceset="C_POL" device="6X6MM" value="100u"/>
<part name="U$13" library="SchlegelAr_Master" deviceset="GND" device=""/>
<part name="U$14" library="SchlegelAr_Master" deviceset="+3V3" device=""/>
<part name="U$15" library="SchlegelAr_Master" deviceset="FRAME_A3_LANDSCAPE" device=""/>
</parts>
<sheets>
<sheet>
<plain>
<text x="35.56" y="210.82" size="2.54" layer="94">Erstellung des Projekts, S2: Blockschaltbild, S3: Spannungsversorgung +3.3V hinzugefuegt</text>
<text x="10.16" y="246.38" size="5.08" layer="94">HDMI_RGB_LVDS</text>
<text x="83.82" y="246.38" size="5.08" layer="94">Masterarbeit Armin Schlegel SS14</text>
<text x="83.82" y="238.76" size="5.08" layer="94">www.armin-schlegel.de</text>
<text x="10.16" y="226.06" size="5.08" layer="94">Changelog</text>
<wire x1="7.62" y1="223.52" x2="33.02" y2="223.52" width="0.6096" layer="94"/>
<wire x1="33.02" y1="223.52" x2="355.6" y2="223.52" width="0.6096" layer="94"/>
<wire x1="7.62" y1="223.52" x2="7.62" y2="215.9" width="0.6096" layer="94"/>
<wire x1="7.62" y1="215.9" x2="7.62" y2="208.28" width="0.6096" layer="94"/>
<wire x1="7.62" y1="208.28" x2="7.62" y2="200.66" width="0.6096" layer="94"/>
<wire x1="7.62" y1="200.66" x2="7.62" y2="193.04" width="0.6096" layer="94"/>
<wire x1="7.62" y1="193.04" x2="7.62" y2="185.42" width="0.6096" layer="94"/>
<wire x1="7.62" y1="185.42" x2="7.62" y2="177.8" width="0.6096" layer="94"/>
<wire x1="7.62" y1="177.8" x2="7.62" y2="170.18" width="0.6096" layer="94"/>
<wire x1="7.62" y1="170.18" x2="7.62" y2="162.56" width="0.6096" layer="94"/>
<wire x1="7.62" y1="162.56" x2="7.62" y2="154.94" width="0.6096" layer="94"/>
<wire x1="7.62" y1="154.94" x2="7.62" y2="147.32" width="0.6096" layer="94"/>
<wire x1="7.62" y1="147.32" x2="7.62" y2="139.7" width="0.6096" layer="94"/>
<wire x1="7.62" y1="139.7" x2="7.62" y2="132.08" width="0.6096" layer="94"/>
<wire x1="7.62" y1="132.08" x2="7.62" y2="124.46" width="0.6096" layer="94"/>
<wire x1="7.62" y1="124.46" x2="7.62" y2="116.84" width="0.6096" layer="94"/>
<wire x1="7.62" y1="116.84" x2="7.62" y2="109.22" width="0.6096" layer="94"/>
<wire x1="7.62" y1="109.22" x2="7.62" y2="101.6" width="0.6096" layer="94"/>
<wire x1="7.62" y1="101.6" x2="7.62" y2="93.98" width="0.6096" layer="94"/>
<wire x1="7.62" y1="93.98" x2="7.62" y2="86.36" width="0.6096" layer="94"/>
<wire x1="7.62" y1="86.36" x2="7.62" y2="78.74" width="0.6096" layer="94"/>
<wire x1="7.62" y1="78.74" x2="7.62" y2="71.12" width="0.6096" layer="94"/>
<wire x1="7.62" y1="71.12" x2="7.62" y2="63.5" width="0.6096" layer="94"/>
<wire x1="7.62" y1="63.5" x2="7.62" y2="55.88" width="0.6096" layer="94"/>
<wire x1="7.62" y1="55.88" x2="7.62" y2="48.26" width="0.6096" layer="94"/>
<wire x1="7.62" y1="48.26" x2="7.62" y2="40.64" width="0.6096" layer="94"/>
<wire x1="7.62" y1="40.64" x2="7.62" y2="33.02" width="0.6096" layer="94"/>
<wire x1="7.62" y1="33.02" x2="33.02" y2="33.02" width="0.6096" layer="94"/>
<wire x1="33.02" y1="33.02" x2="355.6" y2="33.02" width="0.6096" layer="94"/>
<wire x1="355.6" y1="33.02" x2="355.6" y2="40.64" width="0.6096" layer="94"/>
<wire x1="355.6" y1="40.64" x2="355.6" y2="48.26" width="0.6096" layer="94"/>
<wire x1="355.6" y1="48.26" x2="355.6" y2="55.88" width="0.6096" layer="94"/>
<wire x1="355.6" y1="55.88" x2="355.6" y2="63.5" width="0.6096" layer="94"/>
<wire x1="355.6" y1="63.5" x2="355.6" y2="71.12" width="0.6096" layer="94"/>
<wire x1="355.6" y1="71.12" x2="355.6" y2="78.74" width="0.6096" layer="94"/>
<wire x1="355.6" y1="78.74" x2="355.6" y2="86.36" width="0.6096" layer="94"/>
<wire x1="355.6" y1="86.36" x2="355.6" y2="93.98" width="0.6096" layer="94"/>
<wire x1="355.6" y1="93.98" x2="355.6" y2="101.6" width="0.6096" layer="94"/>
<wire x1="355.6" y1="101.6" x2="355.6" y2="109.22" width="0.6096" layer="94"/>
<wire x1="355.6" y1="109.22" x2="355.6" y2="116.84" width="0.6096" layer="94"/>
<wire x1="355.6" y1="116.84" x2="355.6" y2="124.46" width="0.6096" layer="94"/>
<wire x1="355.6" y1="124.46" x2="355.6" y2="132.08" width="0.6096" layer="94"/>
<wire x1="355.6" y1="132.08" x2="355.6" y2="139.7" width="0.6096" layer="94"/>
<wire x1="355.6" y1="139.7" x2="355.6" y2="147.32" width="0.6096" layer="94"/>
<wire x1="355.6" y1="147.32" x2="355.6" y2="154.94" width="0.6096" layer="94"/>
<wire x1="355.6" y1="154.94" x2="355.6" y2="162.56" width="0.6096" layer="94"/>
<wire x1="355.6" y1="162.56" x2="355.6" y2="170.18" width="0.6096" layer="94"/>
<wire x1="355.6" y1="170.18" x2="355.6" y2="177.8" width="0.6096" layer="94"/>
<wire x1="355.6" y1="177.8" x2="355.6" y2="185.42" width="0.6096" layer="94"/>
<wire x1="355.6" y1="185.42" x2="355.6" y2="193.04" width="0.6096" layer="94"/>
<wire x1="355.6" y1="193.04" x2="355.6" y2="200.66" width="0.6096" layer="94"/>
<wire x1="355.6" y1="200.66" x2="355.6" y2="208.28" width="0.6096" layer="94"/>
<wire x1="355.6" y1="208.28" x2="355.6" y2="215.9" width="0.6096" layer="94"/>
<wire x1="355.6" y1="215.9" x2="355.6" y2="223.52" width="0.6096" layer="94"/>
<wire x1="7.62" y1="215.9" x2="355.6" y2="215.9" width="0.6096" layer="94"/>
<wire x1="7.62" y1="208.28" x2="355.6" y2="208.28" width="0.6096" layer="94"/>
<wire x1="7.62" y1="200.66" x2="355.6" y2="200.66" width="0.6096" layer="94"/>
<wire x1="7.62" y1="193.04" x2="355.6" y2="193.04" width="0.6096" layer="94"/>
<wire x1="7.62" y1="185.42" x2="355.6" y2="185.42" width="0.6096" layer="94"/>
<wire x1="7.62" y1="177.8" x2="355.6" y2="177.8" width="0.6096" layer="94"/>
<wire x1="7.62" y1="170.18" x2="355.6" y2="170.18" width="0.6096" layer="94"/>
<wire x1="7.62" y1="162.56" x2="355.6" y2="162.56" width="0.6096" layer="94"/>
<wire x1="7.62" y1="154.94" x2="355.6" y2="154.94" width="0.6096" layer="94"/>
<wire x1="7.62" y1="147.32" x2="355.6" y2="147.32" width="0.6096" layer="94"/>
<wire x1="7.62" y1="139.7" x2="355.6" y2="139.7" width="0.6096" layer="94"/>
<wire x1="7.62" y1="132.08" x2="355.6" y2="132.08" width="0.6096" layer="94"/>
<wire x1="7.62" y1="124.46" x2="355.6" y2="124.46" width="0.6096" layer="94"/>
<wire x1="7.62" y1="116.84" x2="355.6" y2="116.84" width="0.6096" layer="94"/>
<wire x1="7.62" y1="109.22" x2="355.6" y2="109.22" width="0.6096" layer="94"/>
<wire x1="7.62" y1="101.6" x2="355.6" y2="101.6" width="0.6096" layer="94"/>
<wire x1="7.62" y1="93.98" x2="355.6" y2="93.98" width="0.6096" layer="94"/>
<wire x1="7.62" y1="86.36" x2="355.6" y2="86.36" width="0.6096" layer="94"/>
<wire x1="7.62" y1="78.74" x2="355.6" y2="78.74" width="0.6096" layer="94"/>
<wire x1="7.62" y1="71.12" x2="355.6" y2="71.12" width="0.6096" layer="94"/>
<wire x1="7.62" y1="63.5" x2="355.6" y2="63.5" width="0.6096" layer="94"/>
<wire x1="7.62" y1="55.88" x2="355.6" y2="55.88" width="0.6096" layer="94"/>
<wire x1="7.62" y1="48.26" x2="355.6" y2="48.26" width="0.6096" layer="94"/>
<wire x1="7.62" y1="40.64" x2="355.6" y2="40.64" width="0.6096" layer="94"/>
<text x="10.16" y="210.82" size="2.54" layer="94">20.03.2014</text>
<wire x1="33.02" y1="223.52" x2="33.02" y2="33.02" width="0.6096" layer="94"/>
<text x="10.16" y="218.44" size="2.54" layer="94">Datum</text>
<text x="35.56" y="218.44" size="2.54" layer="94">Aenderung</text>
</plain>
<instances>
<instance part="U$1" gate="G$1" x="0" y="0"/>
</instances>
<busses>
</busses>
<nets>
</nets>
</sheet>
<sheet>
<plain>
<wire x1="20.32" y1="231.14" x2="20.32" y2="210.82" width="0.6096" layer="94"/>
<wire x1="20.32" y1="210.82" x2="68.58" y2="210.82" width="0.6096" layer="94"/>
<wire x1="68.58" y1="210.82" x2="68.58" y2="231.14" width="0.6096" layer="94"/>
<wire x1="68.58" y1="231.14" x2="20.32" y2="231.14" width="0.6096" layer="94"/>
<text x="22.86" y="215.9" size="2.54" layer="94">HDMI Connector

FCI 10029449-111RLF</text>
<wire x1="276.86" y1="231.14" x2="276.86" y2="210.82" width="0.6096" layer="94"/>
<wire x1="276.86" y1="210.82" x2="325.12" y2="210.82" width="0.6096" layer="94"/>
<wire x1="325.12" y1="210.82" x2="325.12" y2="231.14" width="0.6096" layer="94"/>
<wire x1="325.12" y1="231.14" x2="276.86" y2="231.14" width="0.6096" layer="94"/>
<text x="279.4" y="215.9" size="2.54" layer="94">LVDS Display

LG LB070WV8-SL01</text>
<wire x1="276.86" y1="203.2" x2="276.86" y2="182.88" width="0.6096" layer="94"/>
<wire x1="276.86" y1="182.88" x2="325.12" y2="182.88" width="0.6096" layer="94"/>
<wire x1="325.12" y1="182.88" x2="325.12" y2="203.2" width="0.6096" layer="94"/>
<wire x1="325.12" y1="203.2" x2="276.86" y2="203.2" width="0.6096" layer="94"/>
<text x="281.94" y="193.04" size="2.54" layer="94">RGB Display</text>
<wire x1="127" y1="231.14" x2="127" y2="210.82" width="0.6096" layer="94"/>
<wire x1="127" y1="210.82" x2="86.36" y2="210.82" width="0.6096" layer="94"/>
<wire x1="86.36" y1="210.82" x2="86.36" y2="231.14" width="0.6096" layer="94"/>
<wire x1="86.36" y1="231.14" x2="127" y2="231.14" width="0.6096" layer="94"/>
<wire x1="195.58" y1="231.14" x2="195.58" y2="210.82" width="0.6096" layer="94"/>
<wire x1="195.58" y1="210.82" x2="154.94" y2="210.82" width="0.6096" layer="94"/>
<wire x1="154.94" y1="210.82" x2="154.94" y2="231.14" width="0.6096" layer="94"/>
<wire x1="154.94" y1="231.14" x2="195.58" y2="231.14" width="0.6096" layer="94"/>
<wire x1="60.96" y1="190.5" x2="60.96" y2="167.64" width="0.6096" layer="94"/>
<wire x1="60.96" y1="167.64" x2="20.32" y2="167.64" width="0.6096" layer="94"/>
<wire x1="20.32" y1="167.64" x2="20.32" y2="190.5" width="0.6096" layer="94"/>
<wire x1="20.32" y1="190.5" x2="60.96" y2="190.5" width="0.6096" layer="94"/>
<wire x1="60.96" y1="157.48" x2="60.96" y2="134.62" width="0.6096" layer="94"/>
<wire x1="60.96" y1="134.62" x2="20.32" y2="134.62" width="0.6096" layer="94"/>
<wire x1="20.32" y1="134.62" x2="20.32" y2="157.48" width="0.6096" layer="94"/>
<wire x1="20.32" y1="157.48" x2="60.96" y2="157.48" width="0.6096" layer="94"/>
<wire x1="60.96" y1="124.46" x2="60.96" y2="101.6" width="0.6096" layer="94"/>
<wire x1="60.96" y1="101.6" x2="20.32" y2="101.6" width="0.6096" layer="94"/>
<wire x1="20.32" y1="101.6" x2="20.32" y2="124.46" width="0.6096" layer="94"/>
<wire x1="20.32" y1="124.46" x2="60.96" y2="124.46" width="0.6096" layer="94"/>
<wire x1="20.32" y1="88.9" x2="20.32" y2="66.04" width="0.6096" layer="94"/>
<wire x1="20.32" y1="66.04" x2="71.12" y2="66.04" width="0.6096" layer="94"/>
<wire x1="71.12" y1="66.04" x2="71.12" y2="88.9" width="0.6096" layer="94"/>
<wire x1="71.12" y1="88.9" x2="20.32" y2="88.9" width="0.6096" layer="94"/>
<text x="22.86" y="71.12" size="2.54" layer="94">Mini USB
Connector

FCI 10033526-N3212LF</text>
<wire x1="78.74" y1="88.9" x2="78.74" y2="66.04" width="0.6096" layer="94"/>
<wire x1="78.74" y1="66.04" x2="137.16" y2="66.04" width="0.6096" layer="94"/>
<wire x1="137.16" y1="66.04" x2="137.16" y2="88.9" width="0.6096" layer="94"/>
<wire x1="137.16" y1="88.9" x2="78.74" y2="88.9" width="0.6096" layer="94"/>
<text x="81.28" y="71.12" size="2.54" layer="94">AVR ISP
Connector

Molex 53261-0671</text>
<wire x1="215.9" y1="231.14" x2="215.9" y2="210.82" width="0.6096" layer="94"/>
<wire x1="215.9" y1="210.82" x2="261.62" y2="210.82" width="0.6096" layer="94"/>
<wire x1="261.62" y1="210.82" x2="261.62" y2="231.14" width="0.6096" layer="94"/>
<wire x1="261.62" y1="231.14" x2="215.9" y2="231.14" width="0.6096" layer="94"/>
<text x="218.44" y="215.9" size="2.54" layer="94">LVDS Connector

JAE FI-X30SSLA-HF</text>
<wire x1="215.9" y1="203.2" x2="215.9" y2="182.88" width="0.6096" layer="94"/>
<wire x1="215.9" y1="182.88" x2="261.62" y2="182.88" width="0.6096" layer="94"/>
<wire x1="261.62" y1="182.88" x2="261.62" y2="203.2" width="0.6096" layer="94"/>
<wire x1="261.62" y1="203.2" x2="215.9" y2="203.2" width="0.6096" layer="94"/>
<text x="218.44" y="187.96" size="2.54" layer="94">RGB Connector

OMRON XF2M-4015-1A</text>
<text x="88.9" y="215.9" size="2.54" layer="94">HDMI RGB Bridge

TI TFP401A</text>
<text x="157.48" y="215.9" size="2.54" layer="94">RGB LVDS Bridge

TI SN75LVDS83B</text>
<text x="22.86" y="175.26" size="2.54" layer="94">I2C EEPROM

Atmel AT2401C</text>
<text x="22.86" y="144.78" size="2.54" layer="94">Atmel ATmega48</text>
<text x="22.86" y="106.68" size="2.54" layer="94">RS232-USB-Bridge

FTDI FT232RL</text>
<text x="40.64" y="195.58" size="2.54" layer="97">Edid data</text>
<text x="134.62" y="223.52" size="2.54" layer="97">RGB Bus</text>
<text x="200.66" y="223.52" size="2.54" layer="97">LVDS</text>
<text x="40.64" y="203.2" size="2.54" layer="97">I2C</text>
<text x="40.64" y="162.56" size="2.54" layer="97">I2C</text>
<text x="40.64" y="129.54" size="2.54" layer="97">UART</text>
<text x="40.64" y="93.98" size="2.54" layer="97">USB</text>
<text x="68.58" y="147.32" size="2.54" layer="97">AVR ISP</text>
<text x="73.66" y="223.52" size="2.54" layer="97">HDMI</text>
<text x="20.32" y="236.22" size="6.4516" layer="97">Blockschaltbild</text>
</plain>
<instances>
<instance part="U$15" gate="G$1" x="0" y="0"/>
</instances>
<busses>
<bus name="B$1">
<segment>
<wire x1="68.58" y1="220.98" x2="86.36" y2="220.98" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$2">
<segment>
<wire x1="127" y1="220.98" x2="154.94" y2="220.98" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$3">
<segment>
<wire x1="195.58" y1="220.98" x2="215.9" y2="220.98" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$4">
<segment>
<wire x1="261.62" y1="220.98" x2="276.86" y2="220.98" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$5">
<segment>
<wire x1="261.62" y1="193.04" x2="276.86" y2="193.04" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$6">
<segment>
<wire x1="215.9" y1="193.04" x2="142.24" y2="193.04" width="1.6764" layer="92"/>
<wire x1="142.24" y1="193.04" x2="142.24" y2="220.98" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$7">
<segment>
<wire x1="38.1" y1="190.5" x2="38.1" y2="210.82" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$8">
<segment>
<wire x1="38.1" y1="157.48" x2="38.1" y2="167.64" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$9">
<segment>
<wire x1="38.1" y1="124.46" x2="38.1" y2="134.62" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$10">
<segment>
<wire x1="38.1" y1="88.9" x2="38.1" y2="101.6" width="1.6764" layer="92"/>
</segment>
</bus>
<bus name="B$11">
<segment>
<wire x1="106.68" y1="88.9" x2="106.68" y2="144.78" width="1.6764" layer="92"/>
<wire x1="106.68" y1="144.78" x2="60.96" y2="144.78" width="1.6764" layer="92"/>
</segment>
</bus>
</busses>
<nets>
</nets>
</sheet>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$2" gate="G$1" x="0" y="0"/>
<instance part="CON1" gate="G$1" x="22.86" y="218.44" smashed="yes">
<attribute name="NAME" x="17.78" y="222.25" size="1.778" layer="95"/>
<attribute name="VALUE" x="16.51" y="210.82" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="C1" gate="G$1" x="68.58" y="213.36" rot="R270"/>
<instance part="IC1" gate="G$1" x="50.8" y="220.98"/>
<instance part="U$7" gate="G$1" x="30.48" y="231.14"/>
<instance part="U$6" gate="G$1" x="50.8" y="231.14"/>
<instance part="U$8" gate="G$1" x="43.18" y="208.28"/>
<instance part="U$9" gate="G$1" x="68.58" y="208.28"/>
<instance part="U$10" gate="G$1" x="30.48" y="208.28"/>
<instance part="U$11" gate="G$1" x="50.8" y="208.28"/>
<instance part="IC2" gate="G$1" x="91.44" y="218.44"/>
<instance part="U$12" gate="G$1" x="91.44" y="208.28"/>
<instance part="C2" gate="G$1" x="124.46" y="213.36" rot="R270"/>
<instance part="U$13" gate="G$1" x="124.46" y="208.28"/>
<instance part="U$14" gate="G$1" x="124.46" y="231.14"/>
</instances>
<busses>
</busses>
<nets>
<net name="VCC_EXT" class="0">
<segment>
<pinref part="CON1" gate="G$1" pin="1"/>
<pinref part="U$7" gate="G$1" pin="VCC_EXT"/>
<wire x1="27.94" y1="220.98" x2="30.48" y2="220.98" width="0.1524" layer="91"/>
<wire x1="30.48" y1="220.98" x2="30.48" y2="231.14" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="AC1"/>
<wire x1="50.8" y1="226.06" x2="50.8" y2="231.14" width="0.1524" layer="91"/>
<pinref part="U$6" gate="G$1" pin="VCC_EXT"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="-"/>
<wire x1="45.72" y1="220.98" x2="43.18" y2="220.98" width="0.1524" layer="91"/>
<wire x1="43.18" y1="220.98" x2="43.18" y2="208.28" width="0.1524" layer="91"/>
<pinref part="U$8" gate="G$1" pin="GND"/>
</segment>
<segment>
<pinref part="C1" gate="G$1" pin="P$2"/>
<pinref part="U$9" gate="G$1" pin="GND"/>
<wire x1="68.58" y1="208.28" x2="68.58" y2="210.82" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="IC2" gate="G$1" pin="GND"/>
<pinref part="U$12" gate="G$1" pin="GND"/>
<wire x1="91.44" y1="210.82" x2="91.44" y2="208.28" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="C2" gate="G$1" pin="P$2"/>
<wire x1="124.46" y1="210.82" x2="124.46" y2="208.28" width="0.1524" layer="91"/>
<pinref part="U$13" gate="G$1" pin="GND"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="+"/>
<wire x1="55.88" y1="220.98" x2="68.58" y2="220.98" width="0.1524" layer="91"/>
<pinref part="C1" gate="G$1" pin="P$1"/>
<wire x1="68.58" y1="218.44" x2="68.58" y2="220.98" width="0.1524" layer="91"/>
<junction x="68.58" y="220.98"/>
<pinref part="IC2" gate="G$1" pin="IN"/>
<wire x1="68.58" y1="220.98" x2="83.82" y2="220.98" width="0.1524" layer="91"/>
</segment>
</net>
<net name="GND_EXT" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="AC2"/>
<wire x1="50.8" y1="215.9" x2="50.8" y2="208.28" width="0.1524" layer="91"/>
<pinref part="U$11" gate="G$1" pin="GND_EXT"/>
</segment>
<segment>
<pinref part="CON1" gate="G$1" pin="3"/>
<pinref part="U$10" gate="G$1" pin="GND_EXT"/>
<wire x1="27.94" y1="215.9" x2="30.48" y2="215.9" width="0.1524" layer="91"/>
<wire x1="30.48" y1="215.9" x2="30.48" y2="208.28" width="0.1524" layer="91"/>
<pinref part="CON1" gate="G$1" pin="2"/>
<wire x1="27.94" y1="218.44" x2="30.48" y2="218.44" width="0.1524" layer="91"/>
<wire x1="30.48" y1="218.44" x2="30.48" y2="215.9" width="0.1524" layer="91"/>
<junction x="30.48" y="215.9"/>
</segment>
</net>
<net name="+3V3" class="0">
<segment>
<pinref part="IC2" gate="G$1" pin="OUT"/>
<wire x1="99.06" y1="220.98" x2="101.6" y2="220.98" width="0.1524" layer="91"/>
<pinref part="IC2" gate="G$1" pin="OUT1"/>
<wire x1="101.6" y1="220.98" x2="124.46" y2="220.98" width="0.1524" layer="91"/>
<wire x1="124.46" y1="220.98" x2="124.46" y2="231.14" width="0.1524" layer="91"/>
<wire x1="99.06" y1="218.44" x2="101.6" y2="218.44" width="0.1524" layer="91"/>
<wire x1="101.6" y1="218.44" x2="101.6" y2="220.98" width="0.1524" layer="91"/>
<junction x="101.6" y="220.98"/>
<pinref part="C2" gate="G$1" pin="P$1"/>
<wire x1="124.46" y1="218.44" x2="124.46" y2="220.98" width="0.1524" layer="91"/>
<junction x="124.46" y="220.98"/>
<pinref part="U$14" gate="G$1" pin="+3V3"/>
</segment>
</net>
</nets>
</sheet>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$3" gate="G$1" x="0" y="0"/>
</instances>
<busses>
</busses>
<nets>
</nets>
</sheet>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$4" gate="G$1" x="0" y="0"/>
</instances>
<busses>
</busses>
<nets>
</nets>
</sheet>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$5" gate="G$1" x="0" y="0"/>
</instances>
<busses>
</busses>
<nets>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="6.3" minversion="6.2.2" severity="warning">
Since Version 6.2.2 text objects can contain more than one line,
which will not be processed correctly with this version.
</note>
</compatibility>
</eagle>
