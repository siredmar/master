//POVRay-Datei erstellt mit 3d41.ulp v20110101
///home/armin/eagle/Masterarbeit/HDMI_RGB_LVDS/HDMI_RGB_LVDS_V10.brd
//7/28/14 6:24 PM

#version 3.5;

//Set to on if the file should be used as .inc
#local use_file_as_inc = off;
#if(use_file_as_inc=off)


//changes the apperance of resistors (1 Blob / 0 real)
#declare global_res_shape = 1;
//randomize color of resistors 1=random 0=same color
#declare global_res_colselect = 0;
//Number of the color for the resistors
//0=Green, 1="normal color" 2=Blue 3=Brown
#declare global_res_col = 1;
//Set to on if you want to render the PCB upside-down
#declare pcb_upsidedown = off;
//Set to x or z to rotate around the corresponding axis (referring to pcb_upsidedown)
#declare pcb_rotdir = x;
//Set the length off short pins over the PCB
#declare pin_length = 2.5;
#declare global_diode_bend_radius = 1;
#declare global_res_bend_radius = 1;
#declare global_solder = on;

#declare global_show_screws = on;
#declare global_show_washers = on;
#declare global_show_nuts = on;

#declare global_use_radiosity = on;

#declare global_ambient_mul = 1;
#declare global_ambient_mul_emit = 0;

//Animation
#declare global_anim = off;
#local global_anim_showcampath = no;

#declare global_fast_mode = off;

#declare col_preset = 2;
#declare pin_short = on;

#declare e3d_environment = off;

#local cam_x = 0;
#local cam_y = 258;
#local cam_z = -138;
#local cam_a = 20;
#local cam_look_x = 0;
#local cam_look_y = -6;
#local cam_look_z = 0;

#local pcb_rotate_x = 180;
#local pcb_rotate_y = 15;
#local pcb_rotate_z = 10;

#local pcb_board = on;
#local pcb_parts = on;
#local pcb_wire_bridges = off;
#if(global_fast_mode=off)
	#local pcb_polygons = on;
	#local pcb_silkscreen = on;
	#local pcb_wires = on;
	#local pcb_pads_smds = on;
#else
	#local pcb_polygons = off;
	#local pcb_silkscreen = off;
	#local pcb_wires = off;
	#local pcb_pads_smds = off;
#end

#local lgt1_pos_x = 26;
#local lgt1_pos_y = 39;
#local lgt1_pos_z = 31;
#local lgt1_intense = 0.760000;
#local lgt2_pos_x = -26;
#local lgt2_pos_y = 39;
#local lgt2_pos_z = 31;
#local lgt2_intense = 0.760000;
#local lgt3_pos_x = 26;
#local lgt3_pos_y = 39;
#local lgt3_pos_z = -21;
#local lgt3_intense = 0.760000;
#local lgt4_pos_x = -26;
#local lgt4_pos_y = 39;
#local lgt4_pos_z = -21;
#local lgt4_intense = 0.760000;

//Do not change these values
#declare pcb_height = 1.500000;
#declare pcb_cuheight = 0.035000;
#declare pcb_x_size = 70.000000;
#declare pcb_y_size = 60.000000;
#declare pcb_layer1_used = 1;
#declare pcb_layer16_used = 1;
#declare inc_testmode = off;
#declare global_seed=seed(321);
#declare global_pcb_layer_dis = array[16]
{
	0.000000,
	0.500000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	0.000000,
	1.000000,
	1.535000,
}
#declare global_pcb_real_hole = 2.000000;

#include "e3d_tools.inc"
#include "e3d_user.inc"

global_settings{charset utf8}

#if(e3d_environment=on)
sky_sphere {pigment {Navy}
pigment {bozo turbulence 0.65 octaves 7 omega 0.7 lambda 2
color_map {
[0.0 0.1 color rgb <0.85, 0.85, 0.85> color rgb <0.75, 0.75, 0.75>]
[0.1 0.5 color rgb <0.75, 0.75, 0.75> color rgbt <1, 1, 1, 1>]
[0.5 1.0 color rgbt <1, 1, 1, 1> color rgbt <1, 1, 1, 1>]}
scale <0.1, 0.5, 0.1>} rotate -90*x}
plane{y, -10.0-max(pcb_x_size,pcb_y_size)*abs(max(sin((pcb_rotate_x/180)*pi),sin((pcb_rotate_z/180)*pi)))
texture{T_Chrome_2D
normal{waves 0.1 frequency 3000.0 scale 3000.0}} translate<0,0,0>}
#end

//Animationsdaten
#if(global_anim=on)
#declare global_anim_showcampath = no;
#end

#if((global_anim=on)|(global_anim_showcampath=yes))
#declare global_anim_npoints_cam_flight=0;
#warning "Keine/zu wenig Animationsdaten vorhanden (mind. 3 Punkte) (Flugpfad)"
#end

#if((global_anim=on)|(global_anim_showcampath=yes))
#declare global_anim_npoints_cam_view=0;
#warning "Keine/zu wenig Animationsdaten vorhanden (mind. 3 Punkte) (Blickpunktpfad)"
#end

#if((global_anim=on)|(global_anim_showcampath=yes))
#end

#if((global_anim_showcampath=yes)&(global_anim=off))
#end
#if(global_anim=on)
camera
{
	location global_anim_spline_cam_flight(clock)
	#if(global_anim_npoints_cam_view>2)
		look_at global_anim_spline_cam_view(clock)
	#else
		look_at global_anim_spline_cam_flight(clock+0.01)-<0,-0.01,0>
	#end
	angle 45
}
light_source
{
	global_anim_spline_cam_flight(clock)
	color rgb <1,1,1>
	spotlight point_at 
	#if(global_anim_npoints_cam_view>2)
		global_anim_spline_cam_view(clock)
	#else
		global_anim_spline_cam_flight(clock+0.01)-<0,-0.01,0>
	#end
	radius 35 falloff  40
}
#else
camera
{
	location <cam_x,cam_y,cam_z>
	look_at <cam_look_x,cam_look_y,cam_look_z>
	angle cam_a
	//versetzt die Kamera, sodass <0,0,0> über dem Eagle-Nullpunkt ist
	//translate<-35.000000,0,-30.000000>
}
#end

background{col_bgr}
light_source{<lgt1_pos_x,lgt1_pos_y,lgt1_pos_z> White*lgt1_intense}
light_source{<lgt2_pos_x,lgt2_pos_y,lgt2_pos_z> White*lgt2_intense}
light_source{<lgt3_pos_x,lgt3_pos_y,lgt3_pos_z> White*lgt3_intense}
light_source{<lgt4_pos_x,lgt4_pos_y,lgt4_pos_z> White*lgt4_intense}
#end


#macro HDMI_RGB_LVDS_V10(mac_x_ver,mac_y_ver,mac_z_ver,mac_x_rot,mac_y_rot,mac_z_rot)
union{
#if(pcb_board = on)
difference{
union{
//Platine
prism{-1.500000,0.000000,8
<0.000000,0.000000><70.000000,0.000000>
<70.000000,0.000000><70.000000,60.000000>
<70.000000,60.000000><0.000000,60.000000>
<0.000000,60.000000><0.000000,0.000000>
texture{col_brd}}
}//End union(PCB)
//Bohrungen(real)/Bauteile
//Bohrungen(real)/Platine
cylinder{<2.000000,1,2.000000><2.000000,-5,2.000000>1.600000 texture{col_hls}}
cylinder{<21.750000,1,52.500000><21.750000,-5,52.500000>1.600000 texture{col_hls}}
cylinder{<68.000000,1,58.000000><68.000000,-5,58.000000>1.600000 texture{col_hls}}
cylinder{<68.000000,1,2.000000><68.000000,-5,2.000000>1.600000 texture{col_hls}}
//Bohrungen(real)/Durchkontaktierungen
}//End difference(reale Bohrungen/Durchbrüche)
#end
#if(pcb_parts=on)//Bauteile
union{
}//End union
#end
#if(pcb_pads_smds=on)
//Lötaugen&SMD/Bauteile
object{TOOLS_PCB_SMD(3.500000,1.600000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<40.550000,-1.537000,53.507500>}
object{TOOLS_PCB_SMD(3.500000,1.600000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<45.950000,-1.537000,53.507500>}
object{TOOLS_PCB_SMD(2.600000,1.600000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<66.000000,-1.537000,46.762500>}
object{TOOLS_PCB_SMD(2.600000,1.600000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<66.000000,-1.537000,50.362500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<53.500000,-1.537000,22.150000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<53.500000,-1.537000,20.850000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<48.022122,-1.537000,18.459619>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<47.102881,-1.537000,17.540378>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-135.000000,0> texture{col_pds} translate<44.022122,-1.537000,10.415378>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-135.000000,0> texture{col_pds} translate<43.102881,-1.537000,11.334619>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<28.500000,0.000000,11.662500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<28.500000,0.000000,12.962500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<28.500000,0.000000,15.212500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<28.500000,0.000000,13.912500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<26.500000,0.000000,9.962500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<26.500000,0.000000,8.662500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<17.500000,-1.537000,9.587500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<17.500000,-1.537000,8.287500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<6.812500,-1.537000,12.162500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<6.812500,-1.537000,13.462500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<8.625000,-1.537000,13.162500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<8.625000,-1.537000,14.462500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-135.000000,0> texture{col_pds} translate<50.959622,-1.537000,22.352878>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-135.000000,0> texture{col_pds} translate<50.040381,-1.537000,23.272119>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-135.000000,0> texture{col_pds} translate<41.959622,-1.537000,15.290378>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-135.000000,0> texture{col_pds} translate<41.040381,-1.537000,16.209619>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<19.212500,0.000000,42.500000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<17.912500,0.000000,42.500000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<20.287500,0.000000,44.375000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<21.587500,0.000000,44.375000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<20.287500,0.000000,42.875000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<21.587500,0.000000,42.875000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<19.212500,0.000000,40.750000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<17.912500,0.000000,40.750000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<19.087500,0.000000,37.375000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<17.787500,0.000000,37.375000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<19.087500,0.000000,35.875000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<17.787500,0.000000,35.875000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<20.100000,0.000000,35.125000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<21.400000,0.000000,35.125000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<20.037500,0.000000,47.625000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<21.337500,0.000000,47.625000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<27.125000,0.000000,12.337500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<27.125000,0.000000,11.037500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<14.125000,0.000000,13.087500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<14.125000,0.000000,11.787500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<45.602881,-1.537000,15.602878>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<46.522122,-1.537000,16.522119>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<43.915381,-1.537000,13.915378>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<44.834622,-1.537000,14.834619>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<44.209622,-1.537000,29.834619>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<43.290381,-1.537000,28.915378>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-315.000000,0> texture{col_pds} translate<49.040381,-1.537000,31.022119>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-315.000000,0> texture{col_pds} translate<49.959622,-1.537000,30.102878>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<54.709622,-1.537000,17.647119>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<53.790381,-1.537000,16.727878>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<48.977881,-1.537000,19.040378>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<49.897122,-1.537000,19.959619>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<38.915381,-1.537000,24.790378>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<39.834622,-1.537000,25.709619>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<37.290381,-1.537000,23.102878>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-45.000000,0> texture{col_pds} translate<38.209622,-1.537000,24.022119>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-135.000000,0> texture{col_pds} translate<38.397122,-1.537000,18.915378>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-135.000000,0> texture{col_pds} translate<37.477881,-1.537000,19.834619>}
object{TOOLS_PCB_SMD(2.600000,1.600000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<9.750000,0.000000,27.762500>}
object{TOOLS_PCB_SMD(2.600000,1.600000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<9.750000,0.000000,31.362500>}
object{TOOLS_PCB_SMD(2.600000,1.600000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<31.862500,0.000000,54.812500>}
object{TOOLS_PCB_SMD(2.600000,1.600000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<28.262500,0.000000,54.812500>}
object{TOOLS_PCB_SMD(2.600000,1.600000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<10.887500,0.000000,51.750000>}
object{TOOLS_PCB_SMD(2.600000,1.600000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<14.487500,0.000000,51.750000>}
#ifndef(global_pack_CON1) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(2.000000,1.000000,1,16,3+global_tmp,100) rotate<0,-270.000000,0>translate<56.400000,0,32.385000> texture{col_thl}}
#ifndef(global_pack_CON1) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(2.000000,1.000000,1,16,3+global_tmp,100) rotate<0,-180.000000,0>translate<59.500000,0,37.185000> texture{col_thl}}
#ifndef(global_pack_CON1) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(2.000000,1.000000,1,16,3+global_tmp,100) rotate<0,-270.000000,0>translate<62.500000,0,32.385000> texture{col_thl}}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<49.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<50.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<50.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<51.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<51.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<52.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<52.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<53.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<53.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<54.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<54.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<55.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<55.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<56.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<56.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<57.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<57.875000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<58.375000,0.000000,7.500000>}
object{TOOLS_PCB_SMD(0.300000,1.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<58.875000,0.000000,7.500000>}
#ifndef(global_pack_CON2) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.700000,1.300000,1,16,1+global_tmp,0) rotate<0,-0.000000,0>translate<46.875000,0,6.550000> texture{col_thl}}
#ifndef(global_pack_CON2) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.700000,1.300000,1,16,1+global_tmp,0) rotate<0,-0.000000,0>translate<46.275000,0,1.650000> texture{col_thl}}
#ifndef(global_pack_CON2) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.700000,1.300000,1,16,1+global_tmp,0) rotate<0,-0.000000,0>translate<61.375000,0,6.550000> texture{col_thl}}
#ifndef(global_pack_CON2) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.700000,1.300000,1,16,1+global_tmp,0) rotate<0,-0.000000,0>translate<61.975000,0,1.650000> texture{col_thl}}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,49.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,48.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,48.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,47.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,47.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,46.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,46.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,45.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,45.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,44.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,44.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,43.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,43.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,42.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,42.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,41.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,41.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,40.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,40.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,39.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,39.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,38.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,38.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,37.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,37.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,36.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,36.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,35.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,35.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,34.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,34.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,33.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,33.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,32.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,32.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,31.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,31.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,30.625000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,30.125000>}
object{TOOLS_PCB_SMD(0.300000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<30.937500,0.000000,29.625000>}
object{TOOLS_PCB_SMD(2.400000,2.400000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<28.237500,0.000000,51.225000>}
object{TOOLS_PCB_SMD(2.400000,2.400000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<28.237500,0.000000,27.525000>}
object{TOOLS_PCB_SMD(0.500000,2.250000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<7.837500,0.000000,7.362500>}
object{TOOLS_PCB_SMD(0.500000,2.250000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<8.637500,0.000000,7.362500>}
object{TOOLS_PCB_SMD(0.500000,2.250000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<9.437500,0.000000,7.362500>}
object{TOOLS_PCB_SMD(0.500000,2.250000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<10.237500,0.000000,7.362500>}
object{TOOLS_PCB_SMD(0.500000,2.250000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<11.037500,0.000000,7.362500>}
object{TOOLS_PCB_SMD(2.000000,2.500000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<4.987500,0.000000,7.362500>}
object{TOOLS_PCB_SMD(2.000000,2.500000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<4.987500,0.000000,1.912500>}
object{TOOLS_PCB_SMD(2.000000,2.500000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<13.887500,0.000000,1.912500>}
object{TOOLS_PCB_SMD(2.000000,2.500000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<13.887500,0.000000,7.362500>}
object{TOOLS_PCB_SMD(0.800000,1.600000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<5.112500,0.000000,21.687500>}
object{TOOLS_PCB_SMD(0.800000,1.600000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<5.112500,0.000000,20.437500>}
object{TOOLS_PCB_SMD(0.800000,1.600000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<5.112500,0.000000,19.187500>}
object{TOOLS_PCB_SMD(0.800000,1.600000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<5.112500,0.000000,17.937500>}
object{TOOLS_PCB_SMD(0.800000,1.600000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<5.112500,0.000000,16.687500>}
object{TOOLS_PCB_SMD(0.800000,1.600000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<5.112500,0.000000,15.437500>}
object{TOOLS_PCB_SMD(2.100000,3.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<2.212500,0.000000,24.312500>}
object{TOOLS_PCB_SMD(2.100000,3.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<2.212500,0.000000,12.812500>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,23.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,24.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,25.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,26.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,27.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,28.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,29.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,30.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,31.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,32.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,33.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,34.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,35.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,36.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,37.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,38.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,39.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,40.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,41.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,42.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,43.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,44.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,45.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,46.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,47.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,48.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,49.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,50.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,51.875000>}
object{TOOLS_PCB_SMD(0.600000,1.500000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<4.712500,-1.537000,52.875000>}
object{TOOLS_PCB_SMD(0.800000,1.500000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<4.712500,-1.537000,22.875000>}
object{TOOLS_PCB_SMD(0.800000,1.500000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<4.712500,-1.537000,53.875000>}
object{TOOLS_PCB_SMD(1.800000,1.800000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<1.612500,-1.537000,19.050000>}
object{TOOLS_PCB_SMD(1.800000,1.800000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<1.612500,-1.537000,57.700000>}
object{TOOLS_PCB_SMD(1.800000,1.800000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<7.812500,-1.537000,19.050000>}
object{TOOLS_PCB_SMD(1.800000,1.800000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<7.812500,-1.537000,57.700000>}
object{TOOLS_PCB_SMD(2.160000,1.070000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<56.225000,0.000000,44.000000>}
object{TOOLS_PCB_SMD(2.160000,1.070000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<57.925000,0.000000,44.000000>}
object{TOOLS_PCB_SMD(2.160000,1.070000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<59.625000,0.000000,44.000000>}
object{TOOLS_PCB_SMD(2.160000,1.070000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<61.325000,0.000000,44.000000>}
object{TOOLS_PCB_SMD(2.160000,1.070000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<63.025000,0.000000,44.000000>}
object{TOOLS_PCB_SMD(10.800000,1.800000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<59.625000,0.000000,56.870000>}
object{TOOLS_PCB_SMD(0.500000,2.200000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<18.845000,-1.537000,14.216500>}
object{TOOLS_PCB_SMD(0.500000,2.200000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<20.105000,-1.537000,14.216500>}
object{TOOLS_PCB_SMD(0.500000,2.200000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<21.375000,-1.537000,14.216500>}
object{TOOLS_PCB_SMD(0.500000,2.200000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<22.645000,-1.537000,14.216500>}
object{TOOLS_PCB_SMD(0.500000,2.200000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<22.655000,-1.537000,7.908500>}
object{TOOLS_PCB_SMD(0.500000,2.200000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<21.385000,-1.537000,7.908500>}
object{TOOLS_PCB_SMD(0.500000,2.200000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<20.115000,-1.537000,7.908500>}
object{TOOLS_PCB_SMD(0.500000,2.200000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<18.845000,-1.537000,7.908500>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<53.649494,0.000000,22.914213>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<53.295941,0.000000,23.267766>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<52.942388,0.000000,23.621319>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<52.588834,0.000000,23.974872>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<52.235281,0.000000,24.328425>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<51.881725,0.000000,24.681978>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<51.528172,0.000000,25.035531>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<51.174619,0.000000,25.389084>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<50.821066,0.000000,25.742641>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<50.467513,0.000000,26.096194>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<50.113959,0.000000,26.449747>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<49.760406,0.000000,26.803300>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<49.406853,0.000000,27.156853>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<49.053300,0.000000,27.510406>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<48.699747,0.000000,27.863959>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<48.346194,0.000000,28.217513>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<47.992641,0.000000,28.571066>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<47.639084,0.000000,28.924619>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<47.285531,0.000000,29.278172>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<46.931978,0.000000,29.631725>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<46.578425,0.000000,29.985281>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<46.224872,0.000000,30.338834>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<45.871319,0.000000,30.692388>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<45.517766,0.000000,31.045941>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<45.164213,0.000000,31.399494>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<42.335784,0.000000,31.399494>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<41.982231,0.000000,31.045941>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<41.628678,0.000000,30.692388>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<41.275125,0.000000,30.338834>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<40.921572,0.000000,29.985281>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<40.568019,0.000000,29.631725>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<40.214466,0.000000,29.278172>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<39.860913,0.000000,28.924619>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<39.507356,0.000000,28.571066>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<39.153803,0.000000,28.217513>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<38.800250,0.000000,27.863959>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<38.446697,0.000000,27.510406>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<38.093144,0.000000,27.156853>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<37.739591,0.000000,26.803300>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<37.386038,0.000000,26.449747>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<37.032484,0.000000,26.096194>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<36.678931,0.000000,25.742641>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<36.325378,0.000000,25.389084>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.971825,0.000000,25.035531>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.618272,0.000000,24.681978>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.264716,0.000000,24.328425>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<34.911163,0.000000,23.974872>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<34.557609,0.000000,23.621319>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<34.204056,0.000000,23.267766>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<33.850503,0.000000,22.914213>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<33.850503,0.000000,20.085784>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<34.204056,0.000000,19.732231>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<34.557609,0.000000,19.378678>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<34.911163,0.000000,19.025125>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.264716,0.000000,18.671572>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.618272,0.000000,18.318019>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.971825,0.000000,17.964466>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<36.325378,0.000000,17.610913>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<36.678931,0.000000,17.257356>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<37.032484,0.000000,16.903803>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<37.386038,0.000000,16.550250>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<37.739591,0.000000,16.196697>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<38.093144,0.000000,15.843144>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<38.446697,0.000000,15.489591>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<38.800250,0.000000,15.136038>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<39.153803,0.000000,14.782484>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<39.507356,0.000000,14.428931>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<39.860913,0.000000,14.075378>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<40.214466,0.000000,13.721825>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<40.568019,0.000000,13.368272>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<40.921572,0.000000,13.014716>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<41.275125,0.000000,12.661163>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<41.628678,0.000000,12.307609>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<41.982231,0.000000,11.954056>}
object{TOOLS_PCB_SMD(1.500000,0.350000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<42.335784,0.000000,11.600503>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<45.164213,0.000000,11.600503>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<45.517766,0.000000,11.954056>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<45.871319,0.000000,12.307609>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<46.224872,0.000000,12.661163>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<46.578425,0.000000,13.014716>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<46.931978,0.000000,13.368272>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<47.285531,0.000000,13.721825>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<47.639084,0.000000,14.075378>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<47.992641,0.000000,14.428931>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<48.346194,0.000000,14.782484>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<48.699747,0.000000,15.136038>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<49.053300,0.000000,15.489591>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<49.406853,0.000000,15.843144>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<49.760406,0.000000,16.196697>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<50.113959,0.000000,16.550250>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<50.467513,0.000000,16.903803>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<50.821066,0.000000,17.257356>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<51.174619,0.000000,17.610913>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<51.528172,0.000000,17.964466>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<51.881725,0.000000,18.318019>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<52.235281,0.000000,18.671572>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<52.588834,0.000000,19.025125>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<52.942388,0.000000,19.378678>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<53.295941,0.000000,19.732231>}
object{TOOLS_PCB_SMD(0.350000,1.500000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<53.649494,0.000000,20.085784>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<25.042600,0.000000,8.262500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<25.042600,0.000000,9.062500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<25.042600,0.000000,9.862500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<25.042600,0.000000,10.662500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<25.042600,0.000000,11.462500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<25.042600,0.000000,12.262500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<25.042600,0.000000,13.062500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<25.042600,0.000000,13.862500>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<23.550000,0.000000,15.355100>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<22.750000,0.000000,15.355100>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<21.950000,0.000000,15.355100>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<21.150000,0.000000,15.355100>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<20.350000,0.000000,15.355100>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<19.550000,0.000000,15.355100>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<18.750000,0.000000,15.355100>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<17.950000,0.000000,15.355100>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<16.457400,0.000000,13.862500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<16.457400,0.000000,13.062500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<16.457400,0.000000,12.262500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<16.457400,0.000000,11.462500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<16.457400,0.000000,10.662500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<16.457400,0.000000,9.862500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<16.457400,0.000000,9.062500>}
object{TOOLS_PCB_SMD(1.270000,0.558800,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<16.457400,0.000000,8.262500>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<17.950000,0.000000,6.769900>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<18.750000,0.000000,6.769900>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<19.550000,0.000000,6.769900>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<20.350000,0.000000,6.769900>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<21.150000,0.000000,6.769900>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<21.950000,0.000000,6.769900>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<22.750000,0.000000,6.769900>}
object{TOOLS_PCB_SMD(0.558800,1.270000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<23.550000,0.000000,6.769900>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<13.850000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<13.200000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<12.550000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<11.900000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<11.250000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<10.600000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<9.950000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<9.300000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<8.650000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<8.000000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<7.350000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<6.700000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<6.050000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<5.400000,-1.537000,3.312500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<5.400000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<6.050000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<6.700000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<7.350000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<8.000000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<8.650000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<9.300000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<9.950000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<10.600000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<11.250000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<11.900000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<12.550000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<13.200000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.400000,1.500000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<13.850000,-1.537000,10.562500>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,47.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,47.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,46.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,46.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,45.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,45.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,44.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,44.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,43.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,43.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,42.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,42.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,41.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,41.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,40.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,40.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,39.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,39.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,38.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,38.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,37.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,37.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,36.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,36.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,35.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,35.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,34.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<23.537500,-1.537000,34.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,34.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,34.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,35.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,35.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,36.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,36.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,37.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,37.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,38.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,38.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,39.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,39.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,40.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,40.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,41.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,41.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,42.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,42.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,43.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,43.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,44.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,44.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,45.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,45.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,46.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,46.625000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,47.125000>}
object{TOOLS_PCB_SMD(0.280000,1.000000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<16.037500,-1.537000,47.625000>}
#ifndef(global_pack_JP1) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<59.270000,0,20.437500> texture{col_thl}}
#ifndef(global_pack_JP1) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<56.730000,0,20.437500> texture{col_thl}}
#ifndef(global_pack_JP2) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<59.270000,0,25.812500> texture{col_thl}}
#ifndef(global_pack_JP2) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<56.730000,0,25.812500> texture{col_thl}}
#ifndef(global_pack_JP3) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-0.000000,0>translate<31.375000,0,21.480000> texture{col_thl}}
#ifndef(global_pack_JP3) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-0.000000,0>translate<31.375000,0,24.020000> texture{col_thl}}
#ifndef(global_pack_JP4) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-270.000000,0>translate<22.085000,0,21.750000> texture{col_thl}}
#ifndef(global_pack_JP4) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-270.000000,0>translate<24.625000,0,21.750000> texture{col_thl}}
#ifndef(global_pack_JP4) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-270.000000,0>translate<27.165000,0,21.750000> texture{col_thl}}
#ifndef(global_pack_JP5) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<22.645000,0,3.375000> texture{col_thl}}
#ifndef(global_pack_JP5) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<20.105000,0,3.375000> texture{col_thl}}
#ifndef(global_pack_JP6) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<27.165000,0,18.000000> texture{col_thl}}
#ifndef(global_pack_JP6) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<24.625000,0,18.000000> texture{col_thl}}
#ifndef(global_pack_JP6) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<22.085000,0,18.000000> texture{col_thl}}
#ifndef(global_pack_JP7) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<18.770000,0,18.000000> texture{col_thl}}
#ifndef(global_pack_JP7) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<16.230000,0,18.000000> texture{col_thl}}
#ifndef(global_pack_JP8) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<18.770000,0,21.750000> texture{col_thl}}
#ifndef(global_pack_JP8) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<16.230000,0,21.750000> texture{col_thl}}
#ifndef(global_pack_JP9) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<34.707500,0,2.375000> texture{col_thl}}
#ifndef(global_pack_JP9) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<32.167500,0,2.375000> texture{col_thl}}
#ifndef(global_pack_JP10) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<40.332500,0,2.375000> texture{col_thl}}
#ifndef(global_pack_JP10) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<37.792500,0,2.375000> texture{col_thl}}
#ifndef(global_pack_JP11) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<29.020000,0,2.312500> texture{col_thl}}
#ifndef(global_pack_JP11) #local global_tmp=0; #else #local global_tmp=100; #end object{TOOLS_PCB_VIA(1.314400,0.914400,1,16,3+global_tmp,100) rotate<0,-90.000000,0>translate<26.480000,0,2.312500> texture{col_thl}}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<65.037500,0.000000,19.625000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<66.337500,0.000000,19.625000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<65.037500,0.000000,21.187500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<66.337500,0.000000,21.187500>}
object{TOOLS_PCB_SMD(5.334000,1.930400,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<31.687500,0.000000,17.201000>}
object{TOOLS_PCB_SMD(5.334000,1.930400,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<31.687500,0.000000,7.549000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<66.650000,0.000000,40.875000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<65.350000,0.000000,40.875000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<31.925000,0.000000,26.512500>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<31.925000,0.000000,28.112500>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<32.725000,0.000000,26.512500>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<32.725000,0.000000,28.112500>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<33.525000,0.000000,26.512500>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<33.525000,0.000000,28.112500>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<34.325000,0.000000,26.512500>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<34.325000,0.000000,28.112500>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.425000,0.000000,35.800000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.825000,0.000000,35.800000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.425000,0.000000,36.600000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.825000,0.000000,36.600000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.425000,0.000000,37.400000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.825000,0.000000,37.400000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.425000,0.000000,38.200000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.825000,0.000000,38.200000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<14.250000,0.000000,22.400000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<14.250000,0.000000,21.100000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<56.600000,0.000000,23.687500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<57.900000,0.000000,23.687500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<56.600000,0.000000,18.250000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<57.900000,0.000000,18.250000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<60.000000,0.000000,8.975000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<60.000000,0.000000,10.275000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.217156,0.000000,31.960784>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<34.085784,0.000000,33.092156>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.782841,0.000000,32.526469>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<34.651469,0.000000,33.657841>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<36.348528,0.000000,33.092156>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.217156,0.000000,34.223528>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<36.914213,0.000000,33.657841>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-225.000000,0> texture{col_pds} translate<35.782841,0.000000,34.789213>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.112500,0.000000,42.425000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.512500,0.000000,42.425000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.112500,0.000000,43.225000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.512500,0.000000,43.225000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.112500,0.000000,44.025000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.512500,0.000000,44.025000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.112500,0.000000,44.825000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.512500,0.000000,44.825000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.112500,0.000000,38.925000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.512500,0.000000,38.925000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.112500,0.000000,39.725000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.512500,0.000000,39.725000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.112500,0.000000,40.525000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.512500,0.000000,40.525000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<36.112500,0.000000,41.325000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<34.512500,0.000000,41.325000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<37.237500,0.000000,49.425000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<35.637500,0.000000,49.425000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<37.237500,0.000000,50.225000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<35.637500,0.000000,50.225000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<37.237500,0.000000,51.025000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<35.637500,0.000000,51.025000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<37.237500,0.000000,51.825000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<35.637500,0.000000,51.825000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<37.237500,0.000000,45.925000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<35.637500,0.000000,45.925000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<37.237500,0.000000,46.725000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<35.637500,0.000000,46.725000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<37.237500,0.000000,47.525000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<35.637500,0.000000,47.525000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<37.237500,0.000000,48.325000>}
object{TOOLS_PCB_SMD(0.500000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<35.637500,0.000000,48.325000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<15.562500,-1.537000,6.275000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<15.562500,-1.537000,4.975000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<24.850000,0.000000,15.687500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<26.150000,0.000000,15.687500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<26.975000,0.000000,25.500000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<28.275000,0.000000,25.500000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<22.525000,0.000000,5.312500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<21.225000,0.000000,5.312500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<20.150000,-1.537000,5.937500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<18.850000,-1.537000,5.937500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<14.375000,0.000000,17.287500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<14.375000,0.000000,18.587500>}
object{TOOLS_PCB_SMD(1.200000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<11.625000,0.000000,14.787500>}
object{TOOLS_PCB_SMD(1.500000,1.600000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<10.625000,0.000000,11.537500>}
object{TOOLS_PCB_SMD(1.200000,1.200000,0.037000,0) rotate<0,-270.000000,0> texture{col_pds} translate<9.625000,0.000000,14.787500>}
object{TOOLS_PCB_SMD(1.300000,1.500000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<38.900000,-1.537000,55.625000>}
object{TOOLS_PCB_SMD(1.300000,1.500000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<40.600000,-1.537000,55.625000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<1.437500,0.000000,6.537500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<1.437500,0.000000,7.837500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<2.750000,0.000000,6.537500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-90.000000,0> texture{col_pds} translate<2.750000,0.000000,7.837500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<62.787500,0.000000,19.625000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<64.087500,0.000000,19.625000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<62.787500,0.000000,21.187500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<64.087500,0.000000,21.187500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<57.912500,-1.537000,2.500000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<59.212500,-1.537000,2.500000>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<57.912500,-1.537000,1.312500>}
object{TOOLS_PCB_SMD(0.700000,0.900000,0.037000,0) rotate<0,-0.000000,0> texture{col_pds} translate<59.212500,-1.537000,1.312500>}
object{TOOLS_PCB_SMD(1.000000,1.400000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<63.825000,0.000000,40.912500>}
object{TOOLS_PCB_SMD(1.000000,1.400000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<61.925000,0.000000,40.912500>}
object{TOOLS_PCB_SMD(1.000000,1.400000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<62.875000,0.000000,38.712500>}
object{TOOLS_PCB_SMD(1.000000,1.400000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<37.200000,-1.537000,53.462500>}
object{TOOLS_PCB_SMD(1.000000,1.400000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<35.300000,-1.537000,53.462500>}
object{TOOLS_PCB_SMD(1.000000,1.400000,0.037000,0) rotate<0,-180.000000,0> texture{col_pds} translate<36.250000,-1.537000,55.662500>}
//Lötaugen/Durchkontaktierungen
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<22.562500,0,43.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<22.312500,0,47.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<22.437500,0,35.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.000000,0,41.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.062500,0,36.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.312500,0,24.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<39.125000,0,26.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<42.062500,0,29.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<49.125000,0,29.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<50.750000,0,23.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<40.375000,0,15.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<36.812500,0,19.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<50.750000,0,19.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<52.500000,0,21.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<14.375000,0,19.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<13.250000,0,21.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<9.125000,0,51.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<47.937500,0,17.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<53.250000,0,17.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<47.062500,0,15.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<45.937500,0,14.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<44.125000,0,12.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<60.125000,0,46.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<61.000000,0,46.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<61.937500,0,46.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<62.875000,0,46.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<58.750000,0,23.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.500000,0,24.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<58.937500,0,18.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<67.312500,0,19.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<8.562500,0,52.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<9.187500,0,53.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<30.875000,0,50.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<31.750000,0,50.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<30.875000,0,51.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<31.750000,0,51.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<18.625000,0,4.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<12.750000,0,14.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.812500,0,11.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<24.875000,0,4.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<14.125000,0,13.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<23.812500,0,12.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.750000,0,9.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<60.062500,0,2.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<60.125000,0,1.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.750000,0,16.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<58.562500,0,9.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<15.562500,0,7.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<57.625000,0,9.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<58.500000,0,10.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<57.625000,0,10.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<6.875000,0,6.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<7.750000,0,12.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<57.687500,0,40.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<58.562500,0,40.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<59.437500,0,40.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<60.312500,0,40.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<52.500000,0,47.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<52.500000,0,46.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<52.500000,0,45.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<67.312500,0,21.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<9.562500,0,24.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<10.562500,0,24.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<8.437500,0,24.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<6.437500,0,24.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<7.437500,0,24.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<20.750000,0,9.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<55.312500,0,2.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<19.812500,0,9.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<56.875000,0,3.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<34.687500,0,5.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<24.125000,0,5.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<15.750000,0,3.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<23.937500,0,9.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<22.687500,0,12.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.625000,0,10.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.500000,0,8.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<6.687500,0,14.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<8.500000,0,15.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<5.875000,0,13.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<15.187500,0,11.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<12.562500,0,11.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<12.312500,0,7.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<10.250000,0,4.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<6.562500,0,1.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<6.562500,0,8.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.187500,0,19.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.250000,0,22.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<6.062500,0,27.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.250000,0,53.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.375000,0,49.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.375000,0,47.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.375000,0,44.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.375000,0,41.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.312500,0,38.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.312500,0,35.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.312500,0,32.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.125000,0,57.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.500000,0,15.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<19.812500,0,41.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<19.562500,0,43.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<20.000000,0,48.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<19.875000,0,36.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<20.062500,0,34.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<24.687500,0,35.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<22.562500,0,37.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<28.500000,0,10.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<38.562500,0,25.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<36.500000,0,23.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.437500,0,18.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<41.000000,0,14.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<52.500000,0,21.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<51.625000,0,23.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<53.125000,0,25.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<52.437500,0,25.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<48.437500,0,30.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<42.625000,0,29.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<48.875000,0,17.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<49.812500,0,18.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<54.125000,0,18.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<50.375000,0,5.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<51.875000,0,5.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<53.375000,0,5.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<54.875000,0,5.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<58.125000,0,5.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<26.250000,0,51.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<26.250000,0,27.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.625000,0,31.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.687500,0,35.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<32.000000,0,49.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<32.000000,0,48.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.812500,0,45.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.875000,0,44.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<14.937500,0,41.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.937500,0,38.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.812500,0,34.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<22.500000,0,41.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<22.250000,0,45.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<14.937500,0,34.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<59.625000,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<60.875000,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<62.187500,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<63.437500,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<64.687500,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<58.312500,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<57.062500,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<55.750000,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<54.500000,0,58.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<47.937500,0,16.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<46.500000,0,15.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<44.687500,0,11.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<45.125000,0,13.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<67.500000,0,40.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<48.750000,0,55.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<48.750000,0,54.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<48.750000,0,53.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<48.750000,0,52.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<48.750000,0,51.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<55.562500,0,46.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<56.437500,0,46.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<57.312500,0,46.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<68.750000,0,54.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<67.750000,0,54.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<66.750000,0,54.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<65.875000,0,54.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<61.937500,0,20.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<11.375000,0,32.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<12.625000,0,32.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<11.375000,0,30.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<12.625000,0,30.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<16.250000,0,52.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.625000,0,52.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<16.937500,0,51.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<26.312500,0,53.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<26.312500,0,54.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.250000,0,53.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<26.312500,0,55.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.375000,0,50.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.375000,0,38.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<50.625000,0,37.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<38.375000,0,37.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<49.187500,0,36.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.375000,0,36.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<50.562500,0,36.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.812500,0,35.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<49.437500,0,35.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.687500,0,32.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<47.875000,0,26.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<36.750000,0,31.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<46.187500,0,26.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<36.375000,0,30.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<44.875000,0,26.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<36.000000,0,28.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<43.687500,0,26.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.062500,0,44.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<48.812500,0,44.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<38.125000,0,44.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<47.750000,0,44.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.062500,0,43.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<46.687500,0,43.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<38.125000,0,42.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<45.687500,0,42.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.187500,0,41.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<44.937500,0,41.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<38.250000,0,40.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<43.875000,0,40.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<37.062500,0,39.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<42.812500,0,39.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<38.375000,0,39.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<41.687500,0,38.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<11.375000,0,27.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<14.437500,0,26.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<12.812500,0,18.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.375000,0,48.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<9.562500,0,22.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<2.937500,0,10.562500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<3.500000,0,9.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<32.812500,0,48.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<32.062500,0,32.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.125000,0,32.625000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<33.562500,0,30.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.562500,0,39.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<28.250000,0,39.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<26.812500,0,38.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.875000,0,37.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.625000,0,37.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<28.500000,0,36.687500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<25.562500,0,39.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<24.562500,0,39.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<26.375000,0,44.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<28.375000,0,42.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.687500,0,42.375000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.750000,0,41.750000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.187500,0,41.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<28.062500,0,40.500000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<24.500000,0,44.000000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<25.250000,0,42.937500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<28.312500,0,48.312500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<28.875000,0,46.812500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.000000,0,47.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.937500,0,45.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.500000,0,46.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.125000,0,45.250000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<18.937500,0,46.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<26.437500,0,46.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.750000,0,44.125000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<27.625000,0,44.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<17.625000,0,45.062500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<25.187500,0,47.187500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<29.625000,0,33.875000> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<38.250000,0,53.437500> texture{col_thl}}
object{TOOLS_PCB_VIA(0.600000,0.300000,1,16,1,0) translate<28.312500,0,33.875000> texture{col_thl}}
#end
#if(pcb_wires=on)
union{
//Signale
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.375000,0.000000,6.475000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.375000,0.000000,5.812500>}
box{<0,0,-0.150000><0.662500,0.035000,0.150000> rotate<0,-90.000000,0> translate<1.375000,0.000000,5.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<1.375000,0.000000,6.475000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<1.437500,0.000000,6.537500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<1.375000,0.000000,6.475000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.437500,0.000000,7.837500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.437500,0.000000,9.062500>}
box{<0,0,-0.150000><1.225000,0.035000,0.150000> rotate<0,90.000000,0> translate<1.437500,0.000000,9.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.612500,-1.535000,19.050000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.687500,-1.535000,19.125000>}
box{<0,0,-0.150000><0.106066,0.035000,0.150000> rotate<0,-44.997030,0> translate<1.612500,-1.535000,19.050000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.375000,0.000000,5.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<2.000000,0.000000,5.187500>}
box{<0,0,-0.150000><0.883883,0.035000,0.150000> rotate<0,44.997030,0> translate<1.375000,0.000000,5.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<2.125000,0.000000,27.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<2.125000,0.000000,48.375000>}
box{<0,0,-0.075000><20.500000,0.035000,0.075000> rotate<0,90.000000,0> translate<2.125000,0.000000,48.375000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<2.212500,0.000000,24.312500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<2.212500,0.000000,23.725000>}
box{<0,0,-0.140000><0.587500,0.035000,0.140000> rotate<0,-90.000000,0> translate<2.212500,0.000000,23.725000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<2.125000,0.000000,48.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<2.625000,0.000000,48.875000>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,-44.997030,0> translate<2.125000,0.000000,48.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<2.750000,0.000000,7.837500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<2.750000,0.000000,9.187500>}
box{<0,0,-0.150000><1.350000,0.035000,0.150000> rotate<0,90.000000,0> translate<2.750000,0.000000,9.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.437500,0.000000,9.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<2.937500,0.000000,10.562500>}
box{<0,0,-0.150000><2.121320,0.035000,0.150000> rotate<0,-44.997030,0> translate<1.437500,0.000000,9.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.612500,-1.535000,57.700000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.112500,-1.535000,57.700000>}
box{<0,0,-0.150000><1.500000,0.035000,0.150000> rotate<0,0.000000,0> translate<1.612500,-1.535000,57.700000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.112500,-1.535000,57.700000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.125000,-1.535000,57.687500>}
box{<0,0,-0.150000><0.017678,0.035000,0.150000> rotate<0,44.997030,0> translate<3.112500,-1.535000,57.700000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<2.750000,0.000000,6.537500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.162500,0.000000,6.537500>}
box{<0,0,-0.150000><0.412500,0.035000,0.150000> rotate<0,0.000000,0> translate<2.750000,0.000000,6.537500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<1.687500,-1.535000,19.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.187500,-1.535000,19.125000>}
box{<0,0,-0.150000><1.500000,0.035000,0.150000> rotate<0,0.000000,0> translate<1.687500,-1.535000,19.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<2.212500,0.000000,23.725000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<3.250000,0.000000,22.687500>}
box{<0,0,-0.140000><1.467247,0.035000,0.140000> rotate<0,44.997030,0> translate<2.212500,0.000000,23.725000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<2.625000,0.000000,48.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<3.375000,0.000000,48.875000>}
box{<0,0,-0.075000><0.750000,0.035000,0.075000> rotate<0,0.000000,0> translate<2.625000,0.000000,48.875000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<3.250000,-1.535000,22.687500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<3.437500,-1.535000,22.875000>}
box{<0,0,-0.140000><0.265165,0.035000,0.140000> rotate<0,-44.997030,0> translate<3.250000,-1.535000,22.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<2.750000,0.000000,9.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.500000,0.000000,9.937500>}
box{<0,0,-0.150000><1.060660,0.035000,0.150000> rotate<0,-44.997030,0> translate<2.750000,0.000000,9.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.162500,0.000000,6.537500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.562500,0.000000,6.937500>}
box{<0,0,-0.150000><0.565685,0.035000,0.150000> rotate<0,-44.997030,0> translate<3.162500,0.000000,6.537500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.562500,0.000000,6.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.562500,0.000000,8.750000>}
box{<0,0,-0.150000><1.812500,0.035000,0.150000> rotate<0,90.000000,0> translate<3.562500,0.000000,8.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<2.125000,0.000000,27.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<3.968750,0.000000,26.031250>}
box{<0,0,-0.075000><2.607456,0.035000,0.075000> rotate<0,44.997030,0> translate<2.125000,0.000000,27.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<3.968750,0.000000,26.031250>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<4.000000,0.000000,26.000000>}
box{<0,0,-0.075000><0.044194,0.035000,0.075000> rotate<0,44.997030,0> translate<3.968750,0.000000,26.031250> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.562500,0.000000,8.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.062500,0.000000,9.250000>}
box{<0,0,-0.150000><0.707107,0.035000,0.150000> rotate<0,-44.997030,0> translate<3.562500,0.000000,8.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.500000,-1.535000,9.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.312500,-1.535000,9.125000>}
box{<0,0,-0.150000><1.149049,0.035000,0.150000> rotate<0,44.997030,0> translate<3.500000,-1.535000,9.937500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<3.437500,-1.535000,22.875000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<4.712500,-1.535000,22.875000>}
box{<0,0,-0.140000><1.275000,0.035000,0.140000> rotate<0,0.000000,0> translate<3.437500,-1.535000,22.875000> }
cylinder{<0,0,0><0,0.035000,0>0.300000 translate<4.712500,-1.535000,23.875000>}
cylinder{<0,0,0><0,0.035000,0>0.300000 translate<4.712500,-1.535000,24.875000>}
box{<0,0,-0.300000><1.000000,0.035000,0.300000> rotate<0,90.000000,0> translate<4.712500,-1.535000,24.875000> }
cylinder{<0,0,0><0,0.035000,0>0.300000 translate<4.712500,-1.535000,25.875000>}
cylinder{<0,0,0><0,0.035000,0>0.300000 translate<4.712500,-1.535000,24.875000>}
box{<0,0,-0.300000><1.000000,0.035000,0.300000> rotate<0,-90.000000,0> translate<4.712500,-1.535000,24.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.312500,-1.535000,32.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,32.875000>}
box{<0,0,-0.150000><1.400000,0.035000,0.150000> rotate<0,0.000000,0> translate<3.312500,-1.535000,32.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.312500,-1.535000,35.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,35.875000>}
box{<0,0,-0.150000><1.400000,0.035000,0.150000> rotate<0,0.000000,0> translate<3.312500,-1.535000,35.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.312500,-1.535000,38.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,38.875000>}
box{<0,0,-0.150000><1.400000,0.035000,0.150000> rotate<0,0.000000,0> translate<3.312500,-1.535000,38.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.375000,-1.535000,41.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,41.875000>}
box{<0,0,-0.150000><1.337500,0.035000,0.150000> rotate<0,0.000000,0> translate<3.375000,-1.535000,41.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.375000,-1.535000,44.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,44.875000>}
box{<0,0,-0.150000><1.337500,0.035000,0.150000> rotate<0,0.000000,0> translate<3.375000,-1.535000,44.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.375000,-1.535000,47.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,47.875000>}
box{<0,0,-0.150000><1.337500,0.035000,0.150000> rotate<0,0.000000,0> translate<3.375000,-1.535000,47.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.375000,-1.535000,48.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,48.875000>}
box{<0,0,-0.150000><1.337500,0.035000,0.150000> rotate<0,0.000000,0> translate<3.375000,-1.535000,48.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.375000,-1.535000,49.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,49.875000>}
box{<0,0,-0.150000><1.337500,0.035000,0.150000> rotate<0,0.000000,0> translate<3.375000,-1.535000,49.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.375000,-1.535000,50.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,50.875000>}
box{<0,0,-0.150000><1.337500,0.035000,0.150000> rotate<0,0.000000,0> translate<3.375000,-1.535000,50.875000> }
cylinder{<0,0,0><0,0.035000,0>0.300000 translate<4.712500,-1.535000,51.875000>}
cylinder{<0,0,0><0,0.035000,0>0.300000 translate<4.712500,-1.535000,52.875000>}
box{<0,0,-0.300000><1.000000,0.035000,0.300000> rotate<0,90.000000,0> translate<4.712500,-1.535000,52.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<3.250000,-1.535000,53.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,53.875000>}
box{<0,0,-0.150000><1.462500,0.035000,0.150000> rotate<0,0.000000,0> translate<3.250000,-1.535000,53.875000> }
cylinder{<0,0,0><0,0.035000,0>0.300000 translate<4.712500,-1.535000,24.875000>}
cylinder{<0,0,0><0,0.035000,0>0.300000 translate<4.775000,-1.535000,24.812500>}
box{<0,0,-0.300000><0.088388,0.035000,0.300000> rotate<0,44.997030,0> translate<4.712500,-1.535000,24.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<4.000000,0.000000,26.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<4.812500,0.000000,25.187500>}
box{<0,0,-0.075000><1.149049,0.035000,0.075000> rotate<0,44.997030,0> translate<4.000000,0.000000,26.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<4.812500,0.000000,23.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<4.812500,0.000000,25.187500>}
box{<0,0,-0.075000><1.437500,0.035000,0.075000> rotate<0,90.000000,0> translate<4.812500,0.000000,25.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<5.000000,0.000000,15.325000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<5.112500,0.000000,15.437500>}
box{<0,0,-0.075000><0.159099,0.035000,0.075000> rotate<0,-44.997030,0> translate<5.000000,0.000000,15.325000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<3.750000,0.000000,16.687500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<5.112500,0.000000,16.687500>}
box{<0,0,-0.152400><1.362500,0.035000,0.152400> rotate<0,0.000000,0> translate<3.750000,0.000000,16.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<2.212500,0.000000,12.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.250000,0.000000,12.812500>}
box{<0,0,-0.150000><3.037500,0.035000,0.150000> rotate<0,0.000000,0> translate<2.212500,0.000000,12.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.987500,0.000000,7.362500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.675000,0.000000,7.362500>}
box{<0,0,-0.150000><0.687500,0.035000,0.150000> rotate<0,0.000000,0> translate<4.987500,0.000000,7.362500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.312500,-1.535000,9.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.687500,-1.535000,9.125000>}
box{<0,0,-0.150000><1.375000,0.035000,0.150000> rotate<0,0.000000,0> translate<4.312500,-1.535000,9.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<4.987500,0.000000,1.912500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<5.837500,0.000000,1.875000>}
box{<0,0,-0.075000><0.850827,0.035000,0.075000> rotate<0,2.525950,0> translate<4.987500,0.000000,1.912500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.250000,0.000000,12.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.875000,0.000000,13.437500>}
box{<0,0,-0.150000><0.883883,0.035000,0.150000> rotate<0,-44.997030,0> translate<5.250000,0.000000,12.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.687500,-1.535000,9.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.000000,-1.535000,9.437500>}
box{<0,0,-0.150000><0.441942,0.035000,0.150000> rotate<0,-44.997030,0> translate<5.687500,-1.535000,9.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.000000,-1.535000,10.512500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.000000,-1.535000,9.437500>}
box{<0,0,-0.150000><1.075000,0.035000,0.150000> rotate<0,-90.000000,0> translate<6.000000,-1.535000,9.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<4.812500,0.000000,23.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<6.000000,0.000000,22.562500>}
box{<0,0,-0.075000><1.679379,0.035000,0.075000> rotate<0,44.997030,0> translate<4.812500,0.000000,23.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,27.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.062500,-1.535000,27.875000>}
box{<0,0,-0.150000><1.350000,0.035000,0.150000> rotate<0,0.000000,0> translate<4.712500,-1.535000,27.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.112500,0.000000,15.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.112500,0.000000,14.437500>}
box{<0,0,-0.150000><1.414214,0.035000,0.150000> rotate<0,44.997030,0> translate<5.112500,0.000000,15.437500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<5.837500,0.000000,1.875000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<6.562500,0.000000,1.875000>}
box{<0,0,-0.152400><0.725000,0.035000,0.152400> rotate<0,0.000000,0> translate<5.837500,0.000000,1.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.675000,0.000000,7.362500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.562500,0.000000,8.250000>}
box{<0,0,-0.150000><1.255115,0.035000,0.150000> rotate<0,-44.997030,0> translate<5.675000,0.000000,7.362500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<5.875000,-1.535000,13.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.600000,-1.535000,13.437500>}
box{<0,0,-0.150000><0.725000,0.035000,0.150000> rotate<0,0.000000,0> translate<5.875000,-1.535000,13.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.112500,0.000000,14.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.687500,0.000000,14.437500>}
box{<0,0,-0.150000><0.575000,0.035000,0.150000> rotate<0,0.000000,0> translate<6.112500,0.000000,14.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.700000,-1.535000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.700000,-1.535000,12.050000>}
box{<0,0,-0.150000><1.487500,0.035000,0.150000> rotate<0,90.000000,0> translate<6.700000,-1.535000,12.050000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.700000,-1.535000,12.050000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.812500,-1.535000,12.162500>}
box{<0,0,-0.150000><0.159099,0.035000,0.150000> rotate<0,-44.997030,0> translate<6.700000,-1.535000,12.050000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<6.700000,-1.535000,12.087500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<6.812500,-1.535000,12.162500>}
box{<0,0,-0.075000><0.135208,0.035000,0.075000> rotate<0,-33.687844,0> translate<6.700000,-1.535000,12.087500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<6.600000,-1.535000,13.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<6.812500,-1.535000,13.462500>}
box{<0,0,-0.075000><0.213966,0.035000,0.075000> rotate<0,-6.709394,0> translate<6.600000,-1.535000,13.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<6.687500,-1.535000,13.525000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<6.812500,-1.535000,13.462500>}
box{<0,0,-0.075000><0.139754,0.035000,0.075000> rotate<0,26.563298,0> translate<6.687500,-1.535000,13.525000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<5.583000,-1.535000,37.124000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<6.835000,-1.535000,37.124000>}
box{<0,0,-0.140000><1.252000,0.035000,0.140000> rotate<0,0.000000,0> translate<5.583000,-1.535000,37.124000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<5.583000,-1.535000,37.574000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<6.835000,-1.535000,37.574000>}
box{<0,0,-0.140000><1.252000,0.035000,0.140000> rotate<0,0.000000,0> translate<5.583000,-1.535000,37.574000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.875000,-1.535000,6.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.000000,-1.535000,6.500000>}
box{<0,0,-0.150000><0.176777,0.035000,0.150000> rotate<0,44.997030,0> translate<6.875000,-1.535000,6.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.875000,0.000000,6.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.212500,0.000000,6.962500>}
box{<0,0,-0.150000><0.477297,0.035000,0.150000> rotate<0,-44.997030,0> translate<6.875000,0.000000,6.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.562500,-1.535000,8.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.350000,-1.535000,9.037500>}
box{<0,0,-0.150000><1.113693,0.035000,0.150000> rotate<0,-44.997030,0> translate<6.562500,-1.535000,8.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.350000,-1.535000,9.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.350000,-1.535000,9.037500>}
box{<0,0,-0.150000><0.400000,0.035000,0.150000> rotate<0,-90.000000,0> translate<7.350000,-1.535000,9.037500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.350000,-1.535000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.350000,-1.535000,9.437500>}
box{<0,0,-0.150000><1.125000,0.035000,0.150000> rotate<0,-90.000000,0> translate<7.350000,-1.535000,9.437500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<5.561000,-1.535000,40.118000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<7.581000,-1.535000,40.118000>}
box{<0,0,-0.140000><2.020000,0.035000,0.140000> rotate<0,0.000000,0> translate<5.561000,-1.535000,40.118000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<5.561000,-1.535000,40.568000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<7.581000,-1.535000,40.568000>}
box{<0,0,-0.140000><2.020000,0.035000,0.140000> rotate<0,0.000000,0> translate<5.561000,-1.535000,40.568000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.350000,-1.535000,9.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.600000,-1.535000,9.187500>}
box{<0,0,-0.150000><0.353553,0.035000,0.150000> rotate<0,44.997030,0> translate<7.350000,-1.535000,9.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.212500,0.000000,6.962500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.775000,0.000000,6.962500>}
box{<0,0,-0.150000><0.562500,0.035000,0.150000> rotate<0,0.000000,0> translate<7.212500,0.000000,6.962500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<7.775000,0.000000,6.962500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<7.837500,0.000000,7.362500>}
box{<0,0,-0.075000><0.404853,0.035000,0.075000> rotate<0,-81.113987,0> translate<7.775000,0.000000,6.962500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.750000,-1.535000,12.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.000000,-1.535000,11.812500>}
box{<0,0,-0.150000><0.353553,0.035000,0.150000> rotate<0,44.997030,0> translate<7.750000,-1.535000,12.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.000000,-1.535000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.000000,-1.535000,11.812500>}
box{<0,0,-0.150000><1.250000,0.035000,0.150000> rotate<0,90.000000,0> translate<8.000000,-1.535000,11.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<2.000000,0.000000,5.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.125000,0.000000,5.187500>}
box{<0,0,-0.150000><6.125000,0.035000,0.150000> rotate<0,0.000000,0> translate<2.000000,0.000000,5.187500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<5.535000,-1.535000,43.095000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<8.430000,-1.535000,43.095000>}
box{<0,0,-0.140000><2.895000,0.035000,0.140000> rotate<0,0.000000,0> translate<5.535000,-1.535000,43.095000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<5.535000,-1.535000,43.545000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<8.430000,-1.535000,43.545000>}
box{<0,0,-0.140000><2.895000,0.035000,0.140000> rotate<0,0.000000,0> translate<5.535000,-1.535000,43.545000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<5.112500,0.000000,21.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<8.437500,0.000000,21.687500>}
box{<0,0,-0.075000><3.325000,0.035000,0.075000> rotate<0,0.000000,0> translate<5.112500,0.000000,21.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.500000,-1.535000,14.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.500000,-1.535000,15.312500>}
box{<0,0,-0.150000><0.687500,0.035000,0.150000> rotate<0,90.000000,0> translate<8.500000,-1.535000,15.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<8.500000,-1.535000,14.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<8.625000,-1.535000,14.462500>}
box{<0,0,-0.075000><0.205015,0.035000,0.075000> rotate<0,52.427948,0> translate<8.500000,-1.535000,14.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<8.600000,-1.535000,14.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<8.625000,-1.535000,14.462500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<8.600000,-1.535000,14.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.500000,0.000000,15.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.625000,0.000000,15.187500>}
box{<0,0,-0.150000><0.176777,0.035000,0.150000> rotate<0,44.997030,0> translate<8.500000,0.000000,15.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.125000,0.000000,5.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.637500,0.000000,5.700000>}
box{<0,0,-0.150000><0.724784,0.035000,0.150000> rotate<0,-44.997030,0> translate<8.125000,0.000000,5.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.637500,0.000000,7.362500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.637500,0.000000,5.700000>}
box{<0,0,-0.150000><1.662500,0.035000,0.150000> rotate<0,-90.000000,0> translate<8.637500,0.000000,5.700000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.000000,-1.535000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.650000,-1.535000,10.562500>}
box{<0,0,-0.150000><0.650000,0.035000,0.150000> rotate<0,0.000000,0> translate<8.000000,-1.535000,10.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.625000,-1.535000,13.162500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.650000,-1.535000,13.137500>}
box{<0,0,-0.150000><0.035355,0.035000,0.150000> rotate<0,44.997030,0> translate<8.625000,-1.535000,13.162500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.650000,-1.535000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.650000,-1.535000,13.137500>}
box{<0,0,-0.150000><2.575000,0.035000,0.150000> rotate<0,90.000000,0> translate<8.650000,-1.535000,13.137500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<8.825000,-1.535000,44.180000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<8.825000,-1.535000,43.940000>}
box{<0,0,-0.140000><0.240000,0.035000,0.140000> rotate<0,-90.000000,0> translate<8.825000,-1.535000,43.940000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.600000,-1.535000,9.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.875000,-1.535000,9.187500>}
box{<0,0,-0.150000><1.275000,0.035000,0.150000> rotate<0,0.000000,0> translate<7.600000,-1.535000,9.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.062500,0.000000,9.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.187500,0.000000,9.250000>}
box{<0,0,-0.150000><5.125000,0.035000,0.150000> rotate<0,0.000000,0> translate<4.062500,0.000000,9.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<5.112500,0.000000,20.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.187500,0.000000,20.437500>}
box{<0,0,-0.075000><4.075000,0.035000,0.075000> rotate<0,0.000000,0> translate<5.112500,0.000000,20.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.625000,0.000000,15.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.225000,0.000000,15.187500>}
box{<0,0,-0.150000><0.600000,0.035000,0.150000> rotate<0,0.000000,0> translate<8.625000,0.000000,15.187500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<9.275000,-1.535000,44.215000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<9.275000,-1.535000,43.940000>}
box{<0,0,-0.140000><0.275000,0.035000,0.140000> rotate<0,-90.000000,0> translate<9.275000,-1.535000,43.940000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.875000,-1.535000,9.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.300000,-1.535000,9.612500>}
box{<0,0,-0.150000><0.601041,0.035000,0.150000> rotate<0,-44.997030,0> translate<8.875000,-1.535000,9.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.300000,-1.535000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.300000,-1.535000,9.612500>}
box{<0,0,-0.150000><0.950000,0.035000,0.150000> rotate<0,-90.000000,0> translate<9.300000,-1.535000,9.612500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.187500,0.000000,9.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.437500,0.000000,9.000000>}
box{<0,0,-0.150000><0.353553,0.035000,0.150000> rotate<0,44.997030,0> translate<9.187500,0.000000,9.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.437500,0.000000,7.362500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.437500,0.000000,9.000000>}
box{<0,0,-0.150000><1.637500,0.035000,0.150000> rotate<0,90.000000,0> translate<9.437500,0.000000,9.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,28.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.500000,-1.535000,28.875000>}
box{<0,0,-0.150000><4.787500,0.035000,0.150000> rotate<0,0.000000,0> translate<4.712500,-1.535000,28.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<6.000000,0.000000,22.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.562500,0.000000,22.562500>}
box{<0,0,-0.075000><3.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<6.000000,0.000000,22.562500> }
cylinder{<0,0,0><0,0.035000,0>0.400000 translate<4.775000,-1.535000,24.812500>}
cylinder{<0,0,0><0,0.035000,0>0.400000 translate<9.562500,-1.535000,24.812500>}
box{<0,0,-0.400000><4.787500,0.035000,0.400000> rotate<0,0.000000,0> translate<4.775000,-1.535000,24.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.225000,0.000000,15.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.625000,0.000000,14.787500>}
box{<0,0,-0.075000><0.565685,0.035000,0.075000> rotate<0,44.997030,0> translate<9.225000,0.000000,15.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<5.112500,0.000000,19.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.937500,0.000000,19.187500>}
box{<0,0,-0.075000><4.825000,0.035000,0.075000> rotate<0,0.000000,0> translate<5.112500,0.000000,19.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.937500,-1.535000,3.325000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.950000,-1.535000,3.312500>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,44.997030,0> translate<9.937500,-1.535000,3.325000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.950000,-1.535000,3.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.950000,-1.535000,4.512500>}
box{<0,0,-0.150000><1.200000,0.035000,0.150000> rotate<0,90.000000,0> translate<9.950000,-1.535000,4.512500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<9.593000,-1.535000,37.856000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<10.014000,-1.535000,37.856000>}
box{<0,0,-0.140000><0.421000,0.035000,0.140000> rotate<0,0.000000,0> translate<9.593000,-1.535000,37.856000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<9.593000,-1.535000,38.306000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<10.014000,-1.535000,38.306000>}
box{<0,0,-0.140000><0.421000,0.035000,0.140000> rotate<0,0.000000,0> translate<9.593000,-1.535000,38.306000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.950000,-1.535000,4.512500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.250000,-1.535000,4.812500>}
box{<0,0,-0.150000><0.424264,0.035000,0.150000> rotate<0,-44.997030,0> translate<9.950000,-1.535000,4.512500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<5.687500,-1.535000,34.087500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<10.348000,-1.535000,33.583500>}
box{<0,0,-0.140000><4.687673,0.035000,0.140000> rotate<0,6.171738,0> translate<5.687500,-1.535000,34.087500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<5.687500,-1.535000,34.537500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<10.348000,-1.535000,34.033500>}
box{<0,0,-0.140000><4.687673,0.035000,0.140000> rotate<0,6.171738,0> translate<5.687500,-1.535000,34.537500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<9.562500,-1.535000,24.812500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<10.562500,-1.535000,24.812500>}
box{<0,0,-0.406400><1.000000,0.035000,0.406400> rotate<0,0.000000,0> translate<9.562500,-1.535000,24.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<5.112500,0.000000,17.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<10.687500,0.000000,17.937500>}
box{<0,0,-0.075000><5.575000,0.035000,0.075000> rotate<0,0.000000,0> translate<5.112500,0.000000,17.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<7.000000,-1.535000,6.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.000000,-1.535000,6.500000>}
box{<0,0,-0.150000><4.000000,0.035000,0.150000> rotate<0,0.000000,0> translate<7.000000,-1.535000,6.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<9.770000,-1.535000,44.710000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.010000,-1.535000,44.710000>}
box{<0,0,-0.140000><1.240000,0.035000,0.140000> rotate<0,0.000000,0> translate<9.770000,-1.535000,44.710000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<9.770000,-1.535000,45.160000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.010000,-1.535000,45.160000>}
box{<0,0,-0.140000><1.240000,0.035000,0.140000> rotate<0,0.000000,0> translate<9.770000,-1.535000,45.160000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.014000,-1.535000,38.983000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.018000,-1.535000,39.100000>}
box{<0,0,-0.140000><0.117068,0.035000,0.140000> rotate<0,-88.036122,0> translate<11.014000,-1.535000,38.983000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<10.975000,0.000000,6.662500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<11.037500,0.000000,7.362500>}
box{<0,0,-0.075000><0.702785,0.035000,0.075000> rotate<0,-84.892232,0> translate<10.975000,0.000000,6.662500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<9.828997,-1.535000,47.343000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.268000,-1.535000,47.343000>}
box{<0,0,-0.140000><1.439003,0.035000,0.140000> rotate<0,0.000000,0> translate<9.828997,-1.535000,47.343000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<9.829000,-1.535000,47.793000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.268000,-1.535000,47.793000>}
box{<0,0,-0.140000><1.439000,0.035000,0.140000> rotate<0,0.000000,0> translate<9.829000,-1.535000,47.793000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<11.250000,-1.535000,3.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<11.312500,-1.535000,3.250000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<11.250000,-1.535000,3.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<11.312500,-1.535000,2.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<11.312500,-1.535000,3.250000>}
box{<0,0,-0.075000><0.875000,0.035000,0.075000> rotate<0,90.000000,0> translate<11.312500,-1.535000,3.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.712500,-1.535000,29.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.312500,-1.535000,29.875000>}
box{<0,0,-0.150000><6.600000,0.035000,0.150000> rotate<0,0.000000,0> translate<4.712500,-1.535000,29.875000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<8.359000,-1.535000,35.818000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.346000,-1.535000,35.821000>}
box{<0,0,-0.140000><2.987002,0.035000,0.140000> rotate<0,-0.057541,0> translate<8.359000,-1.535000,35.818000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<8.359000,-1.535000,36.268000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.346000,-1.535000,36.271000>}
box{<0,0,-0.140000><2.987002,0.035000,0.140000> rotate<0,-0.057541,0> translate<8.359000,-1.535000,36.268000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.500000,-1.535000,28.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.375000,-1.535000,27.000000>}
box{<0,0,-0.150000><2.651650,0.035000,0.150000> rotate<0,44.997030,0> translate<9.500000,-1.535000,28.875000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.464000,-1.535000,38.983000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<11.468000,-1.535000,39.100000>}
box{<0,0,-0.140000><0.117068,0.035000,0.140000> rotate<0,-88.036122,0> translate<11.464000,-1.535000,38.983000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.625000,0.000000,14.787500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.650000,0.000000,14.812500>}
box{<0,0,-0.150000><0.035355,0.035000,0.150000> rotate<0,-44.997030,0> translate<11.625000,0.000000,14.787500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.875000,-1.535000,3.337500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.875000,-1.535000,4.812500>}
box{<0,0,-0.150000><1.475000,0.035000,0.150000> rotate<0,90.000000,0> translate<11.875000,-1.535000,4.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.000000,-1.535000,6.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.875000,-1.535000,5.625000>}
box{<0,0,-0.150000><1.237437,0.035000,0.150000> rotate<0,44.997030,0> translate<11.000000,-1.535000,6.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.875000,-1.535000,4.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.875000,-1.535000,5.625000>}
box{<0,0,-0.150000><0.812500,0.035000,0.150000> rotate<0,90.000000,0> translate<11.875000,-1.535000,5.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.875000,-1.535000,3.337500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.900000,-1.535000,3.312500>}
box{<0,0,-0.150000><0.035355,0.035000,0.150000> rotate<0,44.997030,0> translate<11.875000,-1.535000,3.337500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.625000,0.000000,11.537500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.975000,0.000000,10.187500>}
box{<0,0,-0.150000><1.909188,0.035000,0.150000> rotate<0,44.997030,0> translate<10.625000,0.000000,11.537500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<11.312500,-1.535000,2.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<12.000000,-1.535000,1.687500>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,44.997030,0> translate<11.312500,-1.535000,2.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.875000,-1.535000,4.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<12.062500,-1.535000,5.000000>}
box{<0,0,-0.150000><0.265165,0.035000,0.150000> rotate<0,-44.997030,0> translate<11.875000,-1.535000,4.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.037500,0.000000,7.362500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<12.300000,0.000000,7.362500>}
box{<0,0,-0.150000><1.262500,0.035000,0.150000> rotate<0,0.000000,0> translate<11.037500,0.000000,7.362500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<12.300000,0.000000,7.362500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<12.312500,0.000000,7.375000>}
box{<0,0,-0.150000><0.017678,0.035000,0.150000> rotate<0,-44.997030,0> translate<12.300000,0.000000,7.362500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.900000,-1.535000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<12.550000,-1.535000,10.562500>}
box{<0,0,-0.150000><0.650000,0.035000,0.150000> rotate<0,0.000000,0> translate<11.900000,-1.535000,10.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<12.550000,-1.535000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<12.562500,-1.535000,10.575000>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,-44.997030,0> translate<12.550000,-1.535000,10.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<12.562500,-1.535000,10.575000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<12.562500,-1.535000,11.875000>}
box{<0,0,-0.150000><1.300000,0.035000,0.150000> rotate<0,90.000000,0> translate<12.562500,-1.535000,11.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.650000,0.000000,14.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<12.750000,0.000000,14.812500>}
box{<0,0,-0.150000><1.100000,0.035000,0.150000> rotate<0,0.000000,0> translate<11.650000,0.000000,14.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.562500,-1.535000,22.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<12.812500,-1.535000,19.312500>}
box{<0,0,-0.075000><4.596194,0.035000,0.075000> rotate<0,44.997030,0> translate<9.562500,-1.535000,22.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<12.812500,-1.535000,18.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<12.812500,-1.535000,19.312500>}
box{<0,0,-0.075000><0.875000,0.035000,0.075000> rotate<0,90.000000,0> translate<12.812500,-1.535000,19.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<13.250000,0.000000,21.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<13.275000,0.000000,21.100000>}
box{<0,0,-0.150000><0.035355,0.035000,0.150000> rotate<0,44.997030,0> translate<13.250000,0.000000,21.125000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<9.562500,-1.535000,24.812500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<13.687500,-1.535000,24.812500>}
box{<0,0,-0.406400><4.125000,0.035000,0.406400> rotate<0,0.000000,0> translate<9.562500,-1.535000,24.812500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<12.312500,0.000000,7.375000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<13.875000,0.000000,7.375000>}
box{<0,0,-0.152400><1.562500,0.035000,0.152400> rotate<0,0.000000,0> translate<12.312500,0.000000,7.375000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<13.875000,0.000000,7.375000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<13.887500,0.000000,7.362500>}
box{<0,0,-0.152400><0.017678,0.035000,0.152400> rotate<0,44.997030,0> translate<13.875000,0.000000,7.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.887500,0.000000,1.912500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.937500,0.000000,2.900000>}
box{<0,0,-0.075000><0.988765,0.035000,0.075000> rotate<0,-87.095675,0> translate<13.887500,0.000000,1.912500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.887500,0.000000,7.362500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.937500,0.000000,6.850000>}
box{<0,0,-0.075000><0.514933,0.035000,0.075000> rotate<0,84.422230,0> translate<13.887500,0.000000,7.362500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.937500,0.000000,2.900000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.937500,0.000000,6.850000>}
box{<0,0,-0.075000><3.950000,0.035000,0.075000> rotate<0,90.000000,0> translate<13.937500,0.000000,6.850000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<12.812500,0.000000,18.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.937500,0.000000,17.312500>}
box{<0,0,-0.075000><1.590990,0.035000,0.075000> rotate<0,44.997030,0> translate<12.812500,0.000000,18.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.062500,-1.535000,46.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.062500,-1.535000,47.750000>}
box{<0,0,-0.075000><1.500000,0.035000,0.075000> rotate<0,90.000000,0> translate<14.062500,-1.535000,47.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.125000,0.000000,13.087500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.125000,0.000000,13.937500>}
box{<0,0,-0.150000><0.850000,0.035000,0.150000> rotate<0,90.000000,0> translate<14.125000,0.000000,13.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<10.687500,0.000000,17.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.125000,0.000000,14.500000>}
box{<0,0,-0.075000><4.861359,0.035000,0.075000> rotate<0,44.997030,0> translate<10.687500,0.000000,17.937500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<14.125000,0.000000,13.087500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<14.150000,0.000000,13.062500>}
box{<0,0,-0.140000><0.035355,0.035000,0.140000> rotate<0,44.997030,0> translate<14.125000,0.000000,13.087500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.125000,0.000000,11.787500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.162500,0.000000,11.750000>}
box{<0,0,-0.150000><0.053033,0.035000,0.150000> rotate<0,44.997030,0> translate<14.125000,0.000000,11.787500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.937500,0.000000,19.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.250000,0.000000,14.875000>}
box{<0,0,-0.075000><6.098796,0.035000,0.075000> rotate<0,44.997030,0> translate<9.937500,0.000000,19.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<13.275000,0.000000,21.100000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.250000,0.000000,21.100000>}
box{<0,0,-0.150000><0.975000,0.035000,0.150000> rotate<0,0.000000,0> translate<13.275000,0.000000,21.100000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.375000,0.000000,27.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.250000,0.000000,24.125000>}
box{<0,0,-0.150000><4.065864,0.035000,0.150000> rotate<0,44.997030,0> translate<11.375000,0.000000,27.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.250000,0.000000,22.400000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.250000,0.000000,24.125000>}
box{<0,0,-0.150000><1.725000,0.035000,0.150000> rotate<0,90.000000,0> translate<14.250000,0.000000,24.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.250000,0.000000,22.400000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.275000,0.000000,22.375000>}
box{<0,0,-0.150000><0.035355,0.035000,0.150000> rotate<0,44.997030,0> translate<14.250000,0.000000,22.400000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.937500,0.000000,17.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.350000,0.000000,17.312500>}
box{<0,0,-0.075000><0.412500,0.035000,0.075000> rotate<0,0.000000,0> translate<13.937500,0.000000,17.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.350000,0.000000,17.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.375000,0.000000,17.287500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,44.997030,0> translate<14.350000,0.000000,17.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.375000,0.000000,18.587500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.375000,0.000000,19.500000>}
box{<0,0,-0.150000><0.912500,0.035000,0.150000> rotate<0,90.000000,0> translate<14.375000,0.000000,19.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.975000,0.000000,10.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.437500,0.000000,10.187500>}
box{<0,0,-0.150000><2.462500,0.035000,0.150000> rotate<0,0.000000,0> translate<11.975000,0.000000,10.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<9.187500,0.000000,20.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.437500,0.000000,15.187500>}
box{<0,0,-0.075000><7.424621,0.035000,0.075000> rotate<0,44.997030,0> translate<9.187500,0.000000,20.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<11.312500,-1.535000,29.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.437500,-1.535000,26.750000>}
box{<0,0,-0.150000><4.419417,0.035000,0.150000> rotate<0,44.997030,0> translate<11.312500,-1.535000,29.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.437500,0.000000,26.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.437500,0.000000,26.750000>}
box{<0,0,-0.150000><0.187500,0.035000,0.150000> rotate<0,90.000000,0> translate<14.437500,0.000000,26.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.437500,-1.535000,46.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.437500,-1.535000,47.687500>}
box{<0,0,-0.075000><1.187500,0.035000,0.075000> rotate<0,90.000000,0> translate<14.437500,-1.535000,47.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<8.437500,0.000000,21.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.562500,0.000000,15.562500>}
box{<0,0,-0.075000><8.662058,0.035000,0.075000> rotate<0,44.997030,0> translate<8.437500,0.000000,21.687500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<13.687500,-1.535000,24.812500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<14.750000,-1.535000,23.750000>}
box{<0,0,-0.406400><1.502602,0.035000,0.406400> rotate<0,44.997030,0> translate<13.687500,-1.535000,24.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.812500,-1.535000,47.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.812500,-1.535000,46.937500>}
box{<0,0,-0.075000><0.625000,0.035000,0.075000> rotate<0,-90.000000,0> translate<14.812500,-1.535000,46.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.275000,0.000000,22.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<15.062500,0.000000,22.375000>}
box{<0,0,-0.150000><0.787500,0.035000,0.150000> rotate<0,0.000000,0> translate<14.275000,0.000000,22.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.812500,-1.535000,46.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.125000,-1.535000,46.625000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<14.812500,-1.535000,46.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.162500,0.000000,11.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<15.187500,0.000000,11.750000>}
box{<0,0,-0.150000><1.025000,0.035000,0.150000> rotate<0,0.000000,0> translate<14.162500,0.000000,11.750000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<14.937500,-1.535000,41.375000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<15.187500,-1.535000,41.125000>}
box{<0,0,-0.140000><0.353553,0.035000,0.140000> rotate<0,44.997030,0> translate<14.937500,-1.535000,41.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.062500,-1.535000,46.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.187500,-1.535000,45.125000>}
box{<0,0,-0.075000><1.590990,0.035000,0.075000> rotate<0,44.997030,0> translate<14.062500,-1.535000,46.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.437500,-1.535000,47.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.187500,-1.535000,48.437500>}
box{<0,0,-0.075000><1.060660,0.035000,0.075000> rotate<0,-44.997030,0> translate<14.437500,-1.535000,47.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.437500,-1.535000,46.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.312500,-1.535000,45.625000>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,44.997030,0> translate<14.437500,-1.535000,46.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.812500,-1.535000,47.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.375000,-1.535000,48.125000>}
box{<0,0,-0.075000><0.795495,0.035000,0.075000> rotate<0,-44.997030,0> translate<14.812500,-1.535000,47.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.375000,0.000000,17.287500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.517500,0.000000,17.287500>}
box{<0,0,-0.075000><1.142500,0.035000,0.075000> rotate<0,0.000000,0> translate<14.375000,0.000000,17.287500> }
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<15.562500,-1.535000,5.000000>}
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<15.562500,-1.535000,4.975000>}
box{<0,0,-0.203200><0.025000,0.035000,0.203200> rotate<0,-90.000000,0> translate<15.562500,-1.535000,4.975000> }
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<12.062500,-1.535000,5.000000>}
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<15.562500,-1.535000,5.000000>}
box{<0,0,-0.203200><3.500000,0.035000,0.203200> rotate<0,0.000000,0> translate<12.062500,-1.535000,5.000000> }
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<15.562500,-1.535000,6.275000>}
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<15.562500,-1.535000,7.187500>}
box{<0,0,-0.203200><0.912500,0.035000,0.203200> rotate<0,90.000000,0> translate<15.562500,-1.535000,7.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.437500,0.000000,10.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<15.562500,0.000000,9.062500>}
box{<0,0,-0.150000><1.590990,0.035000,0.150000> rotate<0,44.997030,0> translate<14.437500,0.000000,10.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<15.062500,0.000000,22.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<15.687500,0.000000,21.750000>}
box{<0,0,-0.150000><0.883883,0.035000,0.150000> rotate<0,44.997030,0> translate<15.062500,0.000000,22.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<13.850000,-1.535000,3.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.750000,-1.535000,3.312500>}
box{<0,0,-0.075000><1.900000,0.035000,0.075000> rotate<0,0.000000,0> translate<13.850000,-1.535000,3.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.125000,0.000000,14.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.812500,0.000000,14.500000>}
box{<0,0,-0.075000><1.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<14.125000,0.000000,14.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<14.937500,-1.535000,34.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,34.125000>}
box{<0,0,-0.140000><1.100000,0.035000,0.140000> rotate<0,0.000000,0> translate<14.937500,-1.535000,34.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,37.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,37.125000>}
box{<0,0,-0.140000><0.500000,0.035000,0.140000> rotate<0,-90.000000,0> translate<16.037500,-1.535000,37.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,37.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,37.600000>}
box{<0,0,-0.140000><0.475000,0.035000,0.140000> rotate<0,90.000000,0> translate<16.037500,-1.535000,37.600000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<15.912500,-1.535000,38.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,38.125000>}
box{<0,0,-0.140000><0.125000,0.035000,0.140000> rotate<0,0.000000,0> translate<15.912500,-1.535000,38.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<15.962500,-1.535000,38.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,38.625000>}
box{<0,0,-0.140000><0.075000,0.035000,0.140000> rotate<0,0.000000,0> translate<15.962500,-1.535000,38.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<14.902000,-1.535000,39.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,39.125000>}
box{<0,0,-0.140000><1.135500,0.035000,0.140000> rotate<0,0.000000,0> translate<14.902000,-1.535000,39.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<14.952000,-1.535000,39.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,39.625000>}
box{<0,0,-0.140000><1.085500,0.035000,0.140000> rotate<0,0.000000,0> translate<14.952000,-1.535000,39.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<12.875000,-1.535000,40.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,40.125000>}
box{<0,0,-0.140000><3.162500,0.035000,0.140000> rotate<0,0.000000,0> translate<12.875000,-1.535000,40.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<12.925000,-1.535000,40.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,40.625000>}
box{<0,0,-0.140000><3.112500,0.035000,0.140000> rotate<0,0.000000,0> translate<12.925000,-1.535000,40.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<15.187500,-1.535000,41.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,41.125000>}
box{<0,0,-0.140000><0.850000,0.035000,0.140000> rotate<0,0.000000,0> translate<15.187500,-1.535000,41.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<13.560000,-1.535000,42.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,42.125000>}
box{<0,0,-0.140000><2.477500,0.035000,0.140000> rotate<0,0.000000,0> translate<13.560000,-1.535000,42.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<13.510000,-1.535000,42.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,42.625000>}
box{<0,0,-0.140000><2.527500,0.035000,0.140000> rotate<0,0.000000,0> translate<13.510000,-1.535000,42.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<15.863000,-1.535000,43.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,43.125000>}
box{<0,0,-0.140000><0.174500,0.035000,0.140000> rotate<0,0.000000,0> translate<15.863000,-1.535000,43.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<15.813000,-1.535000,43.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,43.625000>}
box{<0,0,-0.140000><0.224500,0.035000,0.140000> rotate<0,0.000000,0> translate<15.813000,-1.535000,43.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.187500,-1.535000,45.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.037500,-1.535000,45.125000>}
box{<0,0,-0.075000><0.850000,0.035000,0.075000> rotate<0,0.000000,0> translate<15.187500,-1.535000,45.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.312500,-1.535000,45.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.037500,-1.535000,45.625000>}
box{<0,0,-0.075000><0.725000,0.035000,0.075000> rotate<0,0.000000,0> translate<15.312500,-1.535000,45.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.125000,-1.535000,46.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.037500,-1.535000,46.625000>}
box{<0,0,-0.075000><0.912500,0.035000,0.075000> rotate<0,0.000000,0> translate<15.125000,-1.535000,46.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,37.600000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.062500,-1.535000,37.625000>}
box{<0,0,-0.140000><0.035355,0.035000,0.140000> rotate<0,-44.997030,0> translate<16.037500,-1.535000,37.600000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.062500,-1.535000,47.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.062500,-1.535000,49.750000>}
box{<0,0,-0.075000><2.828427,0.035000,0.075000> rotate<0,-44.997030,0> translate<14.062500,-1.535000,47.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.517500,0.000000,17.287500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.230000,0.000000,18.000000>}
box{<0,0,-0.075000><1.007627,0.035000,0.075000> rotate<0,-44.997030,0> translate<15.517500,0.000000,17.287500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<15.687500,0.000000,21.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.230000,0.000000,21.750000>}
box{<0,0,-0.150000><0.542500,0.035000,0.150000> rotate<0,0.000000,0> translate<15.687500,0.000000,21.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.812500,0.000000,14.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.450000,0.000000,13.862500>}
box{<0,0,-0.075000><0.901561,0.035000,0.075000> rotate<0,44.997030,0> translate<15.812500,0.000000,14.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<15.562500,0.000000,9.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.457400,0.000000,9.062500>}
box{<0,0,-0.150000><0.894900,0.035000,0.150000> rotate<0,0.000000,0> translate<15.562500,0.000000,9.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.419900,0.000000,11.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.457400,0.000000,11.462500>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,44.997030,0> translate<16.419900,0.000000,11.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<14.150000,0.000000,13.062500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.457400,0.000000,13.062500>}
box{<0,0,-0.140000><2.307400,0.035000,0.140000> rotate<0,0.000000,0> translate<14.150000,0.000000,13.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.444900,0.000000,13.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.457400,0.000000,13.862500>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,44.997030,0> translate<16.444900,0.000000,13.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.450000,0.000000,13.862500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.457400,0.000000,13.862500>}
box{<0,0,-0.075000><0.007400,0.035000,0.075000> rotate<0,0.000000,0> translate<16.450000,0.000000,13.862500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.457400,0.000000,10.662500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.482400,0.000000,10.687500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<16.457400,0.000000,10.662500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.562500,0.000000,15.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.562500,0.000000,15.562500>}
box{<0,0,-0.075000><2.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<14.562500,0.000000,15.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.037500,-1.535000,36.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.625000,-1.535000,36.125000>}
box{<0,0,-0.150000><0.587500,0.035000,0.150000> rotate<0,0.000000,0> translate<16.037500,-1.535000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.250000,0.000000,14.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.937500,0.000000,14.875000>}
box{<0,0,-0.075000><2.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<14.250000,0.000000,14.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.037500,-1.535000,34.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.000000,-1.535000,34.625000>}
box{<0,0,-0.075000><0.962500,0.035000,0.075000> rotate<0,0.000000,0> translate<16.037500,-1.535000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.037500,-1.535000,41.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.000000,-1.535000,41.625000>}
box{<0,0,-0.150000><0.962500,0.035000,0.150000> rotate<0,0.000000,0> translate<16.037500,-1.535000,41.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.037500,-1.535000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.062500,-1.535000,36.625000>}
box{<0,0,-0.140000><1.025000,0.035000,0.140000> rotate<0,0.000000,0> translate<16.037500,-1.535000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<16.062500,-1.535000,37.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.125000,-1.535000,37.625000>}
box{<0,0,-0.140000><1.062500,0.035000,0.140000> rotate<0,0.000000,0> translate<16.062500,-1.535000,37.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.750000,0.000000,3.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.250000,0.000000,1.812500>}
box{<0,0,-0.075000><2.121320,0.035000,0.075000> rotate<0,44.997030,0> translate<15.750000,0.000000,3.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.037500,-1.535000,44.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.250000,-1.535000,44.625000>}
box{<0,0,-0.075000><1.212500,0.035000,0.075000> rotate<0,0.000000,0> translate<16.037500,-1.535000,44.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.037500,-1.535000,47.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.250000,-1.535000,47.125000>}
box{<0,0,-0.075000><1.212500,0.035000,0.075000> rotate<0,0.000000,0> translate<16.037500,-1.535000,47.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.562500,0.000000,15.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.437500,0.000000,16.437500>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,-44.997030,0> translate<16.562500,0.000000,15.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.419900,0.000000,11.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.500000,0.000000,11.500000>}
box{<0,0,-0.150000><1.080100,0.035000,0.150000> rotate<0,0.000000,0> translate<16.419900,0.000000,11.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.250000,-1.535000,47.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.500000,-1.535000,46.875000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<17.250000,-1.535000,47.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.437500,0.000000,26.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.562500,0.000000,23.437500>}
box{<0,0,-0.150000><4.419417,0.035000,0.150000> rotate<0,44.997030,0> translate<14.437500,0.000000,26.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.037500,-1.535000,46.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.562500,-1.535000,46.125000>}
box{<0,0,-0.150000><1.525000,0.035000,0.150000> rotate<0,0.000000,0> translate<16.037500,-1.535000,46.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.500000,-1.535000,9.587500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.625000,-1.535000,9.712500>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,-44.997030,0> translate<17.500000,-1.535000,9.587500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.482400,0.000000,10.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.625000,0.000000,10.687500>}
box{<0,0,-0.150000><1.142600,0.035000,0.150000> rotate<0,0.000000,0> translate<16.482400,0.000000,10.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.625000,-1.535000,9.712500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.625000,-1.535000,10.687500>}
box{<0,0,-0.150000><0.975000,0.035000,0.150000> rotate<0,90.000000,0> translate<17.625000,-1.535000,10.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.250000,-1.535000,44.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.625000,-1.535000,45.062500>}
box{<0,0,-0.075000><0.576222,0.035000,0.075000> rotate<0,-49.395445,0> translate<17.250000,-1.535000,44.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.037500,-1.535000,47.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.750000,-1.535000,47.625000>}
box{<0,0,-0.075000><1.712500,0.035000,0.075000> rotate<0,0.000000,0> translate<16.037500,-1.535000,47.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<14.437500,0.000000,15.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.782400,0.000000,15.187500>}
box{<0,0,-0.075000><3.344900,0.035000,0.075000> rotate<0,0.000000,0> translate<14.437500,0.000000,15.187500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.062500,0.000000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.787500,0.000000,35.900000>}
box{<0,0,-0.140000><1.025305,0.035000,0.140000> rotate<0,44.997030,0> translate<17.062500,0.000000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.787500,0.000000,35.875000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.787500,0.000000,35.900000>}
box{<0,0,-0.140000><0.025000,0.035000,0.140000> rotate<0,90.000000,0> translate<17.787500,0.000000,35.900000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.062500,0.000000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.787500,0.000000,37.350000>}
box{<0,0,-0.140000><1.025305,0.035000,0.140000> rotate<0,-44.997030,0> translate<17.062500,0.000000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.787500,0.000000,37.375000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.787500,0.000000,37.350000>}
box{<0,0,-0.140000><0.025000,0.035000,0.140000> rotate<0,-90.000000,0> translate<17.787500,0.000000,37.350000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.500000,0.000000,11.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.812500,0.000000,11.812500>}
box{<0,0,-0.150000><0.441942,0.035000,0.150000> rotate<0,-44.997030,0> translate<17.500000,0.000000,11.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.625000,-1.535000,36.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.812500,-1.535000,34.937500>}
box{<0,0,-0.150000><1.679379,0.035000,0.150000> rotate<0,44.997030,0> translate<16.625000,-1.535000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.562500,-1.535000,46.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.812500,-1.535000,45.875000>}
box{<0,0,-0.150000><0.353553,0.035000,0.150000> rotate<0,44.997030,0> translate<17.562500,-1.535000,46.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.000000,0.000000,41.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.875000,0.000000,40.750000>}
box{<0,0,-0.140000><1.237437,0.035000,0.140000> rotate<0,44.997030,0> translate<17.000000,0.000000,41.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.000000,0.000000,41.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.875000,0.000000,42.500000>}
box{<0,0,-0.140000><1.237437,0.035000,0.140000> rotate<0,-44.997030,0> translate<17.000000,0.000000,41.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<16.037500,-1.535000,44.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.875000,-1.535000,44.125000>}
box{<0,0,-0.150000><1.837500,0.035000,0.150000> rotate<0,0.000000,0> translate<16.037500,-1.535000,44.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.875000,0.000000,40.750000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.912500,0.000000,40.750000>}
box{<0,0,-0.140000><0.037500,0.035000,0.140000> rotate<0,0.000000,0> translate<17.875000,0.000000,40.750000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.875000,0.000000,42.500000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.912500,0.000000,42.500000>}
box{<0,0,-0.140000><0.037500,0.035000,0.140000> rotate<0,0.000000,0> translate<17.875000,0.000000,42.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.125000,-1.535000,37.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<17.937500,-1.535000,38.437500>}
box{<0,0,-0.140000><1.149049,0.035000,0.140000> rotate<0,-44.997030,0> translate<17.125000,-1.535000,37.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.782400,0.000000,15.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.950000,0.000000,15.355100>}
box{<0,0,-0.075000><0.237022,0.035000,0.075000> rotate<0,-44.997030,0> translate<17.782400,0.000000,15.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.437500,0.000000,16.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.437500,0.000000,16.437500>}
box{<0,0,-0.075000><1.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<17.437500,0.000000,16.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.500000,-1.535000,8.287500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.466000,-1.535000,8.287500>}
box{<0,0,-0.075000><0.966000,0.035000,0.075000> rotate<0,0.000000,0> translate<17.500000,-1.535000,8.287500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.750000,0.000000,16.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.750000,0.000000,15.355100>}
box{<0,0,-0.075000><0.769900,0.035000,0.075000> rotate<0,-90.000000,0> translate<18.750000,0.000000,15.355100> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.437500,0.000000,16.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.750000,0.000000,16.125000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<18.437500,0.000000,16.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.845000,-1.535000,7.908500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.845000,-1.535000,5.942500>}
box{<0,0,-0.075000><1.966000,0.035000,0.075000> rotate<0,-90.000000,0> translate<18.845000,-1.535000,5.942500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.466000,-1.535000,8.287500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.845000,-1.535000,7.908500>}
box{<0,0,-0.075000><0.535987,0.035000,0.075000> rotate<0,44.997030,0> translate<18.466000,-1.535000,8.287500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.845000,-1.535000,8.655000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.845000,-1.535000,7.908500>}
box{<0,0,-0.075000><0.746500,0.035000,0.075000> rotate<0,-90.000000,0> translate<18.845000,-1.535000,7.908500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.812500,-1.535000,8.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.845000,-1.535000,8.655000>}
box{<0,0,-0.075000><0.045962,0.035000,0.075000> rotate<0,44.997030,0> translate<18.812500,-1.535000,8.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<18.625000,-1.535000,4.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<18.850000,-1.535000,5.162500>}
box{<0,0,-0.150000><0.318198,0.035000,0.150000> rotate<0,-44.997030,0> translate<18.625000,-1.535000,4.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.845000,-1.535000,5.942500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.850000,-1.535000,5.937500>}
box{<0,0,-0.075000><0.007071,0.035000,0.075000> rotate<0,44.997030,0> translate<18.845000,-1.535000,5.942500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<18.850000,-1.535000,5.162500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<18.850000,-1.535000,5.937500>}
box{<0,0,-0.150000><0.775000,0.035000,0.150000> rotate<0,90.000000,0> translate<18.850000,-1.535000,5.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.750000,-1.535000,47.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.937500,-1.535000,46.437500>}
box{<0,0,-0.075000><1.679379,0.035000,0.075000> rotate<0,44.997030,0> translate<17.750000,-1.535000,47.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.000000,-1.535000,34.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.000000,-1.535000,32.625000>}
box{<0,0,-0.075000><2.828427,0.035000,0.075000> rotate<0,44.997030,0> translate<17.000000,-1.535000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.375000,-1.535000,48.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.000000,-1.535000,48.125000>}
box{<0,0,-0.075000><3.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<15.375000,-1.535000,48.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.087500,0.000000,35.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.125000,0.000000,35.875000>}
box{<0,0,-0.075000><0.037500,0.035000,0.075000> rotate<0,0.000000,0> translate<19.087500,0.000000,35.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.087500,0.000000,37.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.125000,0.000000,37.375000>}
box{<0,0,-0.075000><0.037500,0.035000,0.075000> rotate<0,0.000000,0> translate<19.087500,0.000000,37.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.212500,0.000000,40.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.212500,0.000000,41.025000>}
box{<0,0,-0.075000><0.275000,0.035000,0.075000> rotate<0,90.000000,0> translate<19.212500,0.000000,41.025000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.212500,0.000000,42.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.212500,0.000000,42.225000>}
box{<0,0,-0.075000><0.275000,0.035000,0.075000> rotate<0,-90.000000,0> translate<19.212500,0.000000,42.225000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<15.187500,-1.535000,48.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.250000,-1.535000,48.437500>}
box{<0,0,-0.075000><4.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<15.187500,-1.535000,48.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.000000,-1.535000,48.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.500000,-1.535000,47.625000>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,44.997030,0> translate<19.000000,-1.535000,48.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.550000,0.000000,6.769900>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.550000,0.000000,9.425000>}
box{<0,0,-0.075000><2.655100,0.035000,0.075000> rotate<0,90.000000,0> translate<19.550000,0.000000,9.425000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<17.562500,0.000000,23.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<19.625000,0.000000,23.437500>}
box{<0,0,-0.150000><2.062500,0.035000,0.150000> rotate<0,0.000000,0> translate<17.562500,0.000000,23.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.250000,-1.535000,48.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.687500,-1.535000,48.000000>}
box{<0,0,-0.075000><0.618718,0.035000,0.075000> rotate<0,44.997030,0> translate<19.250000,-1.535000,48.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.550000,0.000000,9.425000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.812500,0.000000,9.687500>}
box{<0,0,-0.075000><0.371231,0.035000,0.075000> rotate<0,-44.997030,0> translate<19.550000,0.000000,9.425000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.212500,0.000000,41.025000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.812500,0.000000,41.625000>}
box{<0,0,-0.140000><0.848528,0.035000,0.140000> rotate<0,-44.997030,0> translate<19.212500,0.000000,41.025000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.212500,0.000000,42.225000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.812500,0.000000,41.625000>}
box{<0,0,-0.140000><0.848528,0.035000,0.140000> rotate<0,44.997030,0> translate<19.212500,0.000000,42.225000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.125000,0.000000,35.875000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.875000,0.000000,36.625000>}
box{<0,0,-0.140000><1.060660,0.035000,0.140000> rotate<0,-44.997030,0> translate<19.125000,0.000000,35.875000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.125000,0.000000,37.375000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.875000,0.000000,36.625000>}
box{<0,0,-0.140000><1.060660,0.035000,0.140000> rotate<0,44.997030,0> translate<19.125000,0.000000,37.375000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.000000,0.000000,48.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.037500,0.000000,48.587500>}
box{<0,0,-0.140000><0.053033,0.035000,0.140000> rotate<0,44.997030,0> translate<20.000000,0.000000,48.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.037500,0.000000,47.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.037500,0.000000,48.587500>}
box{<0,0,-0.140000><0.962500,0.035000,0.140000> rotate<0,90.000000,0> translate<20.037500,0.000000,48.587500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.062500,0.000000,34.187500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.100000,0.000000,34.225000>}
box{<0,0,-0.140000><0.053033,0.035000,0.140000> rotate<0,-44.997030,0> translate<20.062500,0.000000,34.187500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.100000,0.000000,35.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.100000,0.000000,34.225000>}
box{<0,0,-0.140000><0.900000,0.035000,0.140000> rotate<0,-90.000000,0> translate<20.100000,0.000000,34.225000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.105000,-1.535000,5.892500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.105000,-1.535000,3.375000>}
box{<0,0,-0.075000><2.517500,0.035000,0.075000> rotate<0,-90.000000,0> translate<20.105000,-1.535000,3.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<18.845000,-1.535000,14.216500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.105000,-1.535000,14.216500>}
box{<0,0,-0.075000><1.260000,0.035000,0.075000> rotate<0,0.000000,0> translate<18.845000,-1.535000,14.216500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.115000,-1.535000,5.972500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.115000,-1.535000,7.908500>}
box{<0,0,-0.075000><1.936000,0.035000,0.075000> rotate<0,90.000000,0> translate<20.115000,-1.535000,7.908500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.105000,-1.535000,5.892500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.150000,-1.535000,5.937500>}
box{<0,0,-0.075000><0.063640,0.035000,0.075000> rotate<0,-44.997030,0> translate<20.105000,-1.535000,5.892500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.115000,-1.535000,5.972500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.150000,-1.535000,5.937500>}
box{<0,0,-0.075000><0.049497,0.035000,0.075000> rotate<0,44.997030,0> translate<20.115000,-1.535000,5.972500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.562500,0.000000,43.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.287500,0.000000,42.900000>}
box{<0,0,-0.140000><1.025305,0.035000,0.140000> rotate<0,44.997030,0> translate<19.562500,0.000000,43.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.287500,0.000000,42.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.287500,0.000000,42.900000>}
box{<0,0,-0.075000><0.025000,0.035000,0.075000> rotate<0,90.000000,0> translate<20.287500,0.000000,42.900000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<19.562500,0.000000,43.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<20.287500,0.000000,44.350000>}
box{<0,0,-0.140000><1.025305,0.035000,0.140000> rotate<0,-44.997030,0> translate<19.562500,0.000000,43.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.287500,0.000000,44.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.287500,0.000000,44.350000>}
box{<0,0,-0.075000><0.025000,0.035000,0.075000> rotate<0,-90.000000,0> translate<20.287500,0.000000,44.350000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.350000,0.000000,6.769900>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.375000,0.000000,6.794900>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<20.350000,0.000000,6.769900> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.375000,0.000000,9.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.375000,0.000000,6.794900>}
box{<0,0,-0.075000><2.330100,0.035000,0.075000> rotate<0,-90.000000,0> translate<20.375000,0.000000,6.794900> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.350000,0.000000,15.355100>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.375000,0.000000,15.380100>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<20.350000,0.000000,15.355100> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.375000,0.000000,16.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.375000,0.000000,15.380100>}
box{<0,0,-0.075000><0.682400,0.035000,0.075000> rotate<0,-90.000000,0> translate<20.375000,0.000000,15.380100> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.250000,0.000000,1.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.500000,0.000000,1.812500>}
box{<0,0,-0.075000><3.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<17.250000,0.000000,1.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.812500,-1.535000,9.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.500000,-1.535000,10.375000>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,-44.997030,0> translate<19.812500,-1.535000,9.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.687500,-1.535000,48.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.625000,-1.535000,48.000000>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<19.687500,-1.535000,48.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.375000,0.000000,16.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.687500,0.000000,16.375000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,-44.997030,0> translate<20.375000,0.000000,16.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.375000,0.000000,9.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.750000,0.000000,9.500000>}
box{<0,0,-0.075000><0.530330,0.035000,0.075000> rotate<0,-44.997030,0> translate<20.375000,0.000000,9.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<19.625000,0.000000,23.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<20.750000,0.000000,22.312500>}
box{<0,0,-0.150000><1.590990,0.035000,0.150000> rotate<0,44.997030,0> translate<19.625000,0.000000,23.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<20.750000,0.000000,18.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<20.750000,0.000000,22.312500>}
box{<0,0,-0.150000><3.750000,0.035000,0.150000> rotate<0,90.000000,0> translate<20.750000,0.000000,22.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.500000,-1.535000,47.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.937500,-1.535000,47.625000>}
box{<0,0,-0.075000><1.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<19.500000,-1.535000,47.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.750000,-1.535000,9.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.125000,-1.535000,9.500000>}
box{<0,0,-0.075000><0.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<20.750000,-1.535000,9.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<21.150000,0.000000,6.769900>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<21.150000,0.000000,5.387500>}
box{<0,0,-0.150000><1.382400,0.035000,0.150000> rotate<0,-90.000000,0> translate<21.150000,0.000000,5.387500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.150000,0.000000,6.769900>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.187500,0.000000,6.807400>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,-44.997030,0> translate<21.150000,0.000000,6.769900> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.187500,0.000000,7.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.187500,0.000000,6.807400>}
box{<0,0,-0.075000><1.005100,0.035000,0.075000> rotate<0,-90.000000,0> translate<21.187500,0.000000,6.807400> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<21.150000,0.000000,5.387500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<21.225000,0.000000,5.312500>}
box{<0,0,-0.150000><0.106066,0.035000,0.150000> rotate<0,44.997030,0> translate<21.150000,0.000000,5.387500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.125000,-1.535000,9.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.312500,-1.535000,9.312500>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,44.997030,0> translate<21.125000,-1.535000,9.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.312500,-1.535000,8.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.312500,-1.535000,9.312500>}
box{<0,0,-0.075000><0.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<21.312500,-1.535000,9.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<20.750000,0.000000,18.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<21.312500,0.000000,18.000000>}
box{<0,0,-0.150000><0.795495,0.035000,0.150000> rotate<0,44.997030,0> translate<20.750000,0.000000,18.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.105000,-1.535000,14.216500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.375000,-1.535000,14.216500>}
box{<0,0,-0.075000><1.270000,0.035000,0.075000> rotate<0,0.000000,0> translate<20.105000,-1.535000,14.216500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.625000,-1.535000,48.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.375000,-1.535000,48.750000>}
box{<0,0,-0.075000><1.060660,0.035000,0.075000> rotate<0,-44.997030,0> translate<20.625000,-1.535000,48.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.385000,-1.535000,7.908500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.385000,-1.535000,6.990000>}
box{<0,0,-0.075000><0.918500,0.035000,0.075000> rotate<0,-90.000000,0> translate<21.385000,-1.535000,6.990000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.312500,-1.535000,8.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.385000,-1.535000,8.615000>}
box{<0,0,-0.075000><0.102530,0.035000,0.075000> rotate<0,44.997030,0> translate<21.312500,-1.535000,8.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.385000,-1.535000,7.908500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.385000,-1.535000,8.615000>}
box{<0,0,-0.075000><0.706500,0.035000,0.075000> rotate<0,90.000000,0> translate<21.385000,-1.535000,8.615000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.500000,0.000000,1.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.500000,0.000000,2.812500>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,-44.997030,0> translate<20.500000,0.000000,1.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.500000,0.000000,4.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.500000,0.000000,2.812500>}
box{<0,0,-0.075000><1.500000,0.035000,0.075000> rotate<0,-90.000000,0> translate<21.500000,0.000000,2.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.587500,0.000000,42.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.587500,0.000000,43.212500>}
box{<0,0,-0.075000><0.337500,0.035000,0.075000> rotate<0,90.000000,0> translate<21.587500,0.000000,43.212500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.587500,0.000000,44.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.587500,0.000000,44.037500>}
box{<0,0,-0.075000><0.337500,0.035000,0.075000> rotate<0,-90.000000,0> translate<21.587500,0.000000,44.037500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.187500,0.000000,7.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,0.000000,8.312500>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,-44.997030,0> translate<21.187500,0.000000,7.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,0.000000,10.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,0.000000,8.312500>}
box{<0,0,-0.075000><1.812500,0.035000,0.075000> rotate<0,-90.000000,0> translate<21.687500,0.000000,8.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.937500,0.000000,14.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,0.000000,10.125000>}
box{<0,0,-0.075000><6.717514,0.035000,0.075000> rotate<0,44.997030,0> translate<16.937500,0.000000,14.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.500000,-1.535000,10.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,-1.535000,10.375000>}
box{<0,0,-0.075000><1.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<20.500000,-1.535000,10.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.937500,-1.535000,47.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,-1.535000,48.375000>}
box{<0,0,-0.075000><1.060660,0.035000,0.075000> rotate<0,-44.997030,0> translate<20.937500,-1.535000,47.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.500000,0.000000,4.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.950000,0.000000,4.762500>}
box{<0,0,-0.075000><0.636396,0.035000,0.075000> rotate<0,-44.997030,0> translate<21.500000,0.000000,4.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.950000,0.000000,6.769900>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.950000,0.000000,4.762500>}
box{<0,0,-0.075000><2.007400,0.035000,0.075000> rotate<0,-90.000000,0> translate<21.950000,0.000000,4.762500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,0.000000,8.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.000000,0.000000,8.625000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,-44.997030,0> translate<21.687500,0.000000,8.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<21.312500,0.000000,18.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<22.085000,0.000000,18.000000>}
box{<0,0,-0.150000><0.772500,0.035000,0.150000> rotate<0,0.000000,0> translate<21.312500,0.000000,18.000000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<21.587500,0.000000,43.212500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.150000,0.000000,43.212500>}
box{<0,0,-0.140000><0.562500,0.035000,0.140000> rotate<0,0.000000,0> translate<21.587500,0.000000,43.212500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<21.587500,0.000000,44.037500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.150000,0.000000,44.037500>}
box{<0,0,-0.140000><0.562500,0.035000,0.140000> rotate<0,0.000000,0> translate<21.587500,0.000000,44.037500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.385000,-1.535000,6.990000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.187500,-1.535000,6.187500>}
box{<0,0,-0.075000><1.134906,0.035000,0.075000> rotate<0,44.997030,0> translate<21.385000,-1.535000,6.990000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<21.337500,0.000000,47.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.312500,0.000000,47.625000>}
box{<0,0,-0.140000><0.975000,0.035000,0.140000> rotate<0,0.000000,0> translate<21.337500,0.000000,47.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<21.400000,0.000000,35.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.437500,0.000000,35.125000>}
box{<0,0,-0.140000><1.037500,0.035000,0.140000> rotate<0,0.000000,0> translate<21.400000,0.000000,35.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.150000,0.000000,43.212500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.562500,0.000000,43.625000>}
box{<0,0,-0.140000><0.583363,0.035000,0.140000> rotate<0,-44.997030,0> translate<22.150000,0.000000,43.212500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.150000,0.000000,44.037500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.562500,0.000000,43.625000>}
box{<0,0,-0.140000><0.583363,0.035000,0.140000> rotate<0,44.997030,0> translate<22.150000,0.000000,44.037500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.375000,-1.535000,14.216500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.645000,-1.535000,14.216500>}
box{<0,0,-0.075000><1.270000,0.035000,0.075000> rotate<0,0.000000,0> translate<21.375000,-1.535000,14.216500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.655000,-1.535000,7.908500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.687500,-1.535000,7.941000>}
box{<0,0,-0.075000><0.045962,0.035000,0.075000> rotate<0,-44.997030,0> translate<22.655000,-1.535000,7.908500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.687500,-1.535000,9.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.687500,-1.535000,7.941000>}
box{<0,0,-0.075000><1.434000,0.035000,0.075000> rotate<0,-90.000000,0> translate<22.687500,-1.535000,7.941000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,-1.535000,10.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.687500,-1.535000,9.375000>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,44.997030,0> translate<21.687500,-1.535000,10.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.687500,-1.535000,14.174000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.687500,-1.535000,12.562500>}
box{<0,0,-0.075000><1.611500,0.035000,0.075000> rotate<0,-90.000000,0> translate<22.687500,-1.535000,12.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.645000,-1.535000,14.216500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.687500,-1.535000,14.174000>}
box{<0,0,-0.075000><0.060104,0.035000,0.075000> rotate<0,44.997030,0> translate<22.645000,-1.535000,14.216500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.750000,0.000000,6.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.750000,0.000000,6.769900>}
box{<0,0,-0.075000><0.394900,0.035000,0.075000> rotate<0,90.000000,0> translate<22.750000,0.000000,6.769900> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.525000,0.000000,5.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.937500,0.000000,5.312500>}
box{<0,0,-0.075000><0.412500,0.035000,0.075000> rotate<0,0.000000,0> translate<22.525000,0.000000,5.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.937500,0.000000,6.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.937500,0.000000,6.062500>}
box{<0,0,-0.075000><0.125000,0.035000,0.075000> rotate<0,-90.000000,0> translate<22.937500,0.000000,6.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.750000,0.000000,6.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.937500,0.000000,6.187500>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,44.997030,0> translate<22.750000,0.000000,6.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<12.000000,-1.535000,1.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.250000,-1.535000,1.687500>}
box{<0,0,-0.075000><11.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<12.000000,-1.535000,1.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.000000,0.000000,8.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.312500,0.000000,8.625000>}
box{<0,0,-0.075000><1.312500,0.035000,0.075000> rotate<0,0.000000,0> translate<22.000000,0.000000,8.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.437500,-1.535000,35.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<23.537500,-1.535000,35.125000>}
box{<0,0,-0.140000><1.100000,0.035000,0.140000> rotate<0,0.000000,0> translate<22.437500,-1.535000,35.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.562500,-1.535000,37.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<23.537500,-1.535000,37.625000>}
box{<0,0,-0.140000><0.975000,0.035000,0.140000> rotate<0,0.000000,0> translate<22.562500,-1.535000,37.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.500000,-1.535000,41.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<23.537500,-1.535000,41.625000>}
box{<0,0,-0.140000><1.037500,0.035000,0.140000> rotate<0,0.000000,0> translate<22.500000,-1.535000,41.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.562500,-1.535000,43.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<23.537500,-1.535000,43.625000>}
box{<0,0,-0.140000><0.975000,0.035000,0.140000> rotate<0,0.000000,0> translate<22.562500,-1.535000,43.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.250000,-1.535000,45.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<23.537500,-1.535000,45.625000>}
box{<0,0,-0.140000><1.287500,0.035000,0.140000> rotate<0,0.000000,0> translate<22.250000,-1.535000,45.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<22.312500,-1.535000,47.625000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<23.537500,-1.535000,47.625000>}
box{<0,0,-0.140000><1.225000,0.035000,0.140000> rotate<0,0.000000,0> translate<22.312500,-1.535000,47.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,43.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.600000,-1.535000,43.187500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<23.537500,-1.535000,43.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,47.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.600000,-1.535000,47.062500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<23.537500,-1.535000,47.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.937500,0.000000,5.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.812500,0.000000,4.437500>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,44.997030,0> translate<22.937500,0.000000,5.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.687500,-1.535000,7.941000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.934000,-1.535000,7.941000>}
box{<0,0,-0.075000><1.246500,0.035000,0.075000> rotate<0,0.000000,0> translate<22.687500,-1.535000,7.941000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.937500,0.000000,6.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.000000,0.000000,5.000000>}
box{<0,0,-0.075000><1.502602,0.035000,0.075000> rotate<0,44.997030,0> translate<22.937500,0.000000,6.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,38.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.062500,-1.535000,38.625000>}
box{<0,0,-0.075000><0.525000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,38.625000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<23.812500,0.000000,12.562500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<24.112500,0.000000,12.262500>}
box{<0,0,-0.152400><0.424264,0.035000,0.152400> rotate<0,44.997030,0> translate<23.812500,0.000000,12.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.250000,-1.535000,1.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.125000,-1.535000,2.562500>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,-44.997030,0> translate<23.250000,-1.535000,1.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.000000,0.000000,5.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.125000,0.000000,5.000000>}
box{<0,0,-0.075000><0.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<24.000000,0.000000,5.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.125000,-1.535000,2.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.125000,-1.535000,5.000000>}
box{<0,0,-0.075000><2.437500,0.035000,0.075000> rotate<0,90.000000,0> translate<24.125000,-1.535000,5.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<20.687500,0.000000,16.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.162500,0.000000,16.375000>}
box{<0,0,-0.075000><3.475000,0.035000,0.075000> rotate<0,0.000000,0> translate<20.687500,0.000000,16.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.062500,-1.535000,38.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.187500,-1.535000,38.500000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<24.062500,-1.535000,38.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,44.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.375000,-1.535000,44.125000>}
box{<0,0,-0.075000><0.837500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,44.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,37.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.500000,-1.535000,37.125000>}
box{<0,0,-0.075000><0.962500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,37.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.375000,-1.535000,44.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.500000,-1.535000,44.000000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<24.375000,-1.535000,44.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,35.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,35.625000>}
box{<0,0,-0.075000><1.025000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,35.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,36.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,36.125000>}
box{<0,0,-0.075000><1.025000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,36.625000>}
box{<0,0,-0.075000><1.025000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,39.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,39.125000>}
box{<0,0,-0.075000><1.025000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,39.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,42.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,42.125000>}
box{<0,0,-0.075000><1.025000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,42.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.500000,0.000000,44.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,0.000000,43.937500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<24.500000,0.000000,44.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,46.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.625000,-1.535000,46.125000>}
box{<0,0,-0.075000><1.087500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,46.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,35.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.687500,-1.535000,35.500000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<24.562500,-1.535000,35.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,41.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.687500,-1.535000,41.125000>}
box{<0,0,-0.075000><1.150000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,41.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<22.187500,-1.535000,6.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.750000,-1.535000,6.187500>}
box{<0,0,-0.075000><2.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<22.187500,-1.535000,6.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,42.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.750000,-1.535000,41.937500>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,44.997030,0> translate<24.562500,-1.535000,42.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,42.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.750000,-1.535000,42.625000>}
box{<0,0,-0.075000><1.212500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,42.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,40.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.812500,-1.535000,40.625000>}
box{<0,0,-0.075000><1.275000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,40.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,44.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.812500,-1.535000,44.625000>}
box{<0,0,-0.075000><1.275000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,44.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.162500,0.000000,16.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.850000,0.000000,15.687500>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,44.997030,0> translate<24.162500,0.000000,16.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.812500,0.000000,4.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.875000,0.000000,4.437500>}
box{<0,0,-0.075000><1.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.812500,0.000000,4.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,45.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.875000,-1.535000,45.125000>}
box{<0,0,-0.075000><1.337500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,45.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.687500,-1.535000,41.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.937500,-1.535000,41.375000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,-44.997030,0> translate<24.687500,-1.535000,41.125000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<23.937500,0.000000,9.937500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<24.967600,0.000000,9.937500>}
box{<0,0,-0.152400><1.030100,0.035000,0.152400> rotate<0,0.000000,0> translate<23.937500,0.000000,9.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.500000,-1.535000,37.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.000000,-1.535000,37.625000>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,-44.997030,0> translate<24.500000,-1.535000,37.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.750000,-1.535000,42.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.000000,-1.535000,42.375000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<24.750000,-1.535000,42.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.600000,-1.535000,43.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.000000,-1.535000,43.187500>}
box{<0,0,-0.075000><1.400000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.600000,-1.535000,43.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.967600,0.000000,9.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.042600,0.000000,9.862500>}
box{<0,0,-0.075000><0.106066,0.035000,0.075000> rotate<0,44.997030,0> translate<24.967600,0.000000,9.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.017600,0.000000,11.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.042600,0.000000,11.462500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.017600,0.000000,11.437500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<24.112500,0.000000,12.262500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<25.042600,0.000000,12.262500>}
box{<0,0,-0.152400><0.930100,0.035000,0.152400> rotate<0,0.000000,0> translate<24.112500,0.000000,12.262500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.992600,0.000000,12.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.042600,0.000000,12.262500>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,44.997030,0> translate<24.992600,0.000000,12.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,36.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.062500,-1.535000,36.625000>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,-44.997030,0> translate<24.562500,-1.535000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,-1.535000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.062500,-1.535000,37.125000>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,-44.997030,0> translate<24.562500,-1.535000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.042600,0.000000,10.662500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.067600,0.000000,10.687500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.042600,0.000000,10.662500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.042600,0.000000,12.262500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.092600,0.000000,12.312500>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.042600,0.000000,12.262500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.042600,0.000000,13.862500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.092600,0.000000,13.912500>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.042600,0.000000,13.862500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.812500,-1.535000,40.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.125000,-1.535000,40.937500>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,-44.997030,0> translate<24.812500,-1.535000,40.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,47.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.125000,-1.535000,47.125000>}
box{<0,0,-0.075000><1.587500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,47.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.042600,0.000000,12.262500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.142600,0.000000,12.362500>}
box{<0,0,-0.075000><0.141421,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.042600,0.000000,12.262500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.042600,0.000000,13.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.142600,0.000000,12.962500>}
box{<0,0,-0.075000><0.141421,0.035000,0.075000> rotate<0,44.997030,0> translate<25.042600,0.000000,13.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.125000,-1.535000,47.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.187500,-1.535000,47.187500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.125000,-1.535000,47.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.187500,-1.535000,38.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.250000,-1.535000,38.500000>}
box{<0,0,-0.075000><1.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<24.187500,-1.535000,38.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.000000,-1.535000,43.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.250000,-1.535000,42.937500>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<25.000000,-1.535000,43.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.187500,0.000000,47.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.250000,0.000000,47.187500>}
box{<0,0,-0.075000><0.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<25.187500,0.000000,47.187500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<25.142600,0.000000,12.962500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<25.275000,0.000000,12.962500>}
box{<0,0,-0.140000><0.132400,0.035000,0.140000> rotate<0,0.000000,0> translate<25.142600,0.000000,12.962500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,40.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.375000,-1.535000,40.125000>}
box{<0,0,-0.075000><1.837500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,40.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,0.000000,43.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.375000,0.000000,43.937500>}
box{<0,0,-0.075000><0.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<24.562500,0.000000,43.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.375000,0.000000,47.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.375000,0.000000,46.187500>}
box{<0,0,-0.075000><0.875000,0.035000,0.075000> rotate<0,-90.000000,0> translate<25.375000,0.000000,46.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.250000,0.000000,47.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.375000,0.000000,47.062500>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<25.250000,0.000000,47.187500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<25.275000,0.000000,12.962500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<25.437500,0.000000,13.125000>}
box{<0,0,-0.140000><0.229810,0.035000,0.140000> rotate<0,-44.997030,0> translate<25.275000,0.000000,12.962500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.437500,0.000000,33.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.437500,0.000000,25.937500>}
box{<0,0,-0.075000><7.062500,0.035000,0.075000> rotate<0,-90.000000,0> translate<25.437500,0.000000,25.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.375000,-1.535000,40.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.562500,-1.535000,39.937500>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,44.997030,0> translate<25.375000,-1.535000,40.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.562500,0.000000,39.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.687500,0.000000,39.812500>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<25.562500,0.000000,39.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.687500,0.000000,38.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.687500,0.000000,39.812500>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,90.000000,0> translate<25.687500,0.000000,39.812500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<25.042600,0.000000,10.662500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<25.800000,0.000000,10.662500>}
box{<0,0,-0.152400><0.757400,0.035000,0.152400> rotate<0,0.000000,0> translate<25.042600,0.000000,10.662500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,38.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.812500,-1.535000,38.125000>}
box{<0,0,-0.075000><2.275000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,38.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.250000,-1.535000,38.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.812500,-1.535000,39.062500>}
box{<0,0,-0.075000><0.795495,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.250000,-1.535000,38.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.437500,0.000000,25.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.875000,0.000000,25.500000>}
box{<0,0,-0.075000><0.618718,0.035000,0.075000> rotate<0,44.997030,0> translate<25.437500,0.000000,25.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,46.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.875000,-1.535000,46.625000>}
box{<0,0,-0.075000><2.337500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,46.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.687500,-1.535000,48.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.875000,-1.535000,48.375000>}
box{<0,0,-0.075000><4.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<21.687500,-1.535000,48.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.125000,-1.535000,40.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.062500,-1.535000,40.937500>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<25.125000,-1.535000,40.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.375000,0.000000,46.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.062500,0.000000,45.500000>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,44.997030,0> translate<25.375000,0.000000,46.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.750000,-1.535000,41.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.125000,-1.535000,41.937500>}
box{<0,0,-0.075000><1.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<24.750000,-1.535000,41.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.625000,0.000000,18.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.150000,0.000000,16.475000>}
box{<0,0,-0.075000><2.156676,0.035000,0.075000> rotate<0,44.997030,0> translate<24.625000,0.000000,18.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.150000,0.000000,15.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.150000,0.000000,16.475000>}
box{<0,0,-0.075000><0.787500,0.035000,0.075000> rotate<0,90.000000,0> translate<26.150000,0.000000,16.475000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.812500,-1.535000,38.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.187500,-1.535000,38.500000>}
box{<0,0,-0.075000><0.530330,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.812500,-1.535000,38.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.937500,-1.535000,41.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.250000,-1.535000,41.375000>}
box{<0,0,-0.075000><1.312500,0.035000,0.075000> rotate<0,0.000000,0> translate<24.937500,-1.535000,41.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.000000,-1.535000,42.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.250000,-1.535000,42.375000>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<25.000000,-1.535000,42.375000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<26.250000,0.000000,51.250000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<26.275000,0.000000,51.225000>}
box{<0,0,-0.406400><0.035355,0.035000,0.406400> rotate<0,44.997030,0> translate<26.250000,0.000000,51.250000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<26.250000,0.000000,27.562500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<26.287500,0.000000,27.525000>}
box{<0,0,-0.406400><0.053033,0.035000,0.406400> rotate<0,44.997030,0> translate<26.250000,0.000000,27.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.625000,-1.535000,46.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.375000,-1.535000,44.375000>}
box{<0,0,-0.075000><2.474874,0.035000,0.075000> rotate<0,44.997030,0> translate<24.625000,-1.535000,46.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.875000,-1.535000,46.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.437500,-1.535000,46.062500>}
box{<0,0,-0.075000><0.795495,0.035000,0.075000> rotate<0,44.997030,0> translate<25.875000,-1.535000,46.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.312500,0.000000,8.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.480000,0.000000,5.457500>}
box{<0,0,-0.075000><4.479521,0.035000,0.075000> rotate<0,44.997030,0> translate<23.312500,0.000000,8.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.480000,0.000000,2.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.480000,0.000000,5.457500>}
box{<0,0,-0.075000><3.145000,0.035000,0.075000> rotate<0,90.000000,0> translate<26.480000,0.000000,5.457500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<25.800000,0.000000,10.662500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<26.500000,0.000000,9.962500>}
box{<0,0,-0.152400><0.989949,0.035000,0.152400> rotate<0,44.997030,0> translate<25.800000,0.000000,10.662500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.500000,0.000000,8.662500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.525000,0.000000,8.687500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<26.500000,0.000000,8.662500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.250000,0.000000,42.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.562500,0.000000,41.625000>}
box{<0,0,-0.075000><1.856155,0.035000,0.075000> rotate<0,44.997030,0> translate<25.250000,0.000000,42.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.562500,0.000000,40.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.562500,0.000000,41.625000>}
box{<0,0,-0.075000><1.562500,0.035000,0.075000> rotate<0,90.000000,0> translate<26.562500,0.000000,41.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.437500,0.000000,46.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.562500,0.000000,45.937500>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<26.437500,0.000000,46.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<17.625000,0.000000,45.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.625000,0.000000,45.062500>}
box{<0,0,-0.075000><9.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<17.625000,0.000000,45.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<21.375000,-1.535000,48.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.625000,-1.535000,48.750000>}
box{<0,0,-0.075000><5.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<21.375000,-1.535000,48.750000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<25.042600,0.000000,11.462500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<26.700000,0.000000,11.462500>}
box{<0,0,-0.140000><1.657400,0.035000,0.140000> rotate<0,0.000000,0> translate<25.042600,0.000000,11.462500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.187500,-1.535000,38.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.812500,-1.535000,38.500000>}
box{<0,0,-0.075000><0.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<26.187500,-1.535000,38.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.437500,0.000000,33.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.875000,0.000000,34.437500>}
box{<0,0,-0.075000><2.032932,0.035000,0.075000> rotate<0,-44.997030,0> translate<25.437500,0.000000,33.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.250000,-1.535000,42.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.875000,-1.535000,41.750000>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,44.997030,0> translate<26.250000,-1.535000,42.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<16.062500,-1.535000,49.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.875000,-1.535000,49.750000>}
box{<0,0,-0.075000><10.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<16.062500,-1.535000,49.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.125000,-1.535000,41.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.937500,-1.535000,41.125000>}
box{<0,0,-0.075000><1.149049,0.035000,0.075000> rotate<0,44.997030,0> translate<26.125000,-1.535000,41.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.875000,0.000000,25.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.975000,0.000000,25.500000>}
box{<0,0,-0.075000><1.100000,0.035000,0.075000> rotate<0,0.000000,0> translate<25.875000,0.000000,25.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.975000,0.000000,25.025000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.975000,0.000000,25.500000>}
box{<0,0,-0.075000><0.475000,0.035000,0.075000> rotate<0,90.000000,0> translate<26.975000,0.000000,25.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.750000,-1.535000,6.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.980000,-1.535000,3.957500>}
box{<0,0,-0.075000><3.153696,0.035000,0.075000> rotate<0,44.997030,0> translate<24.750000,-1.535000,6.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.562500,0.000000,40.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,39.625000>}
box{<0,0,-0.075000><0.618718,0.035000,0.075000> rotate<0,44.997030,0> translate<26.562500,0.000000,40.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.375000,0.000000,43.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,42.312500>}
box{<0,0,-0.075000><2.298097,0.035000,0.075000> rotate<0,44.997030,0> translate<25.375000,0.000000,43.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,40.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,42.312500>}
box{<0,0,-0.075000><2.000000,0.035000,0.075000> rotate<0,90.000000,0> translate<27.000000,0.000000,42.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.062500,0.000000,45.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,45.500000>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<26.062500,0.000000,45.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.875000,-1.535000,48.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,-1.535000,47.250000>}
box{<0,0,-0.075000><1.590990,0.035000,0.075000> rotate<0,44.997030,0> translate<25.875000,-1.535000,48.375000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<25.042600,0.000000,12.262500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.050000,0.000000,12.262500>}
box{<0,0,-0.152400><2.007400,0.035000,0.152400> rotate<0,0.000000,0> translate<25.042600,0.000000,12.262500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.875000,-1.535000,45.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.062500,-1.535000,42.937500>}
box{<0,0,-0.075000><3.093592,0.035000,0.075000> rotate<0,44.997030,0> translate<24.875000,-1.535000,45.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.562500,0.000000,45.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.062500,0.000000,45.937500>}
box{<0,0,-0.075000><0.500000,0.035000,0.075000> rotate<0,0.000000,0> translate<26.562500,0.000000,45.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.950000,0.000000,12.262500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.075000,0.000000,12.387500>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,-44.997030,0> translate<26.950000,0.000000,12.262500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<26.700000,0.000000,11.462500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<27.125000,0.000000,11.037500>}
box{<0,0,-0.140000><0.601041,0.035000,0.140000> rotate<0,44.997030,0> translate<26.700000,0.000000,11.462500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.050000,0.000000,12.262500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.125000,0.000000,12.337500>}
box{<0,0,-0.152400><0.106066,0.035000,0.152400> rotate<0,-44.997030,0> translate<27.050000,0.000000,12.262500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.075000,0.000000,12.387500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.125000,0.000000,12.337500>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,44.997030,0> translate<27.075000,0.000000,12.387500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.100000,0.000000,12.362500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.125000,0.000000,12.337500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,44.997030,0> translate<27.100000,0.000000,12.362500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.250000,-1.535000,41.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.125000,-1.535000,40.500000>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,44.997030,0> translate<26.250000,-1.535000,41.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.812500,-1.535000,44.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.125000,-1.535000,42.312500>}
box{<0,0,-0.075000><3.270369,0.035000,0.075000> rotate<0,44.997030,0> translate<24.812500,-1.535000,44.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<27.165000,0.000000,18.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<27.165000,0.000000,21.750000>}
box{<0,0,-0.150000><3.750000,0.035000,0.150000> rotate<0,90.000000,0> translate<27.165000,0.000000,21.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.375000,0.000000,44.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.250000,0.000000,43.500000>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,44.997030,0> translate<26.375000,0.000000,44.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.934000,-1.535000,7.941000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.312500,-1.535000,4.562500>}
box{<0,0,-0.075000><4.777921,0.035000,0.075000> rotate<0,44.997030,0> translate<23.934000,-1.535000,7.941000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.062500,-1.535000,40.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.312500,-1.535000,39.687500>}
box{<0,0,-0.075000><1.767767,0.035000,0.075000> rotate<0,44.997030,0> translate<26.062500,-1.535000,40.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,40.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.375000,0.000000,39.937500>}
box{<0,0,-0.075000><0.530330,0.035000,0.075000> rotate<0,44.997030,0> translate<27.000000,0.000000,40.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.975000,0.000000,25.025000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.437500,0.000000,24.562500>}
box{<0,0,-0.075000><0.654074,0.035000,0.075000> rotate<0,44.997030,0> translate<26.975000,0.000000,25.025000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,47.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.437500,0.000000,47.250000>}
box{<0,0,-0.075000><0.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<27.000000,0.000000,47.250000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<26.525000,0.000000,8.687500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.500000,0.000000,8.687500>}
box{<0,0,-0.152400><0.975000,0.035000,0.152400> rotate<0,0.000000,0> translate<26.525000,0.000000,8.687500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.500000,0.000000,15.250000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.537500,0.000000,15.212500>}
box{<0,0,-0.152400><0.053033,0.035000,0.152400> rotate<0,44.997030,0> translate<27.500000,0.000000,15.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.625000,0.000000,45.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.625000,0.000000,44.062500>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,44.997030,0> translate<26.625000,0.000000,45.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.000000,-1.535000,37.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.687500,-1.535000,37.625000>}
box{<0,0,-0.075000><2.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<25.000000,-1.535000,37.625000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<26.500000,0.000000,9.962500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.725000,0.000000,9.962500>}
box{<0,0,-0.152400><1.225000,0.035000,0.152400> rotate<0,0.000000,0> translate<26.500000,0.000000,9.962500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.725000,0.000000,9.962500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.750000,0.000000,9.937500>}
box{<0,0,-0.152400><0.035355,0.035000,0.152400> rotate<0,44.997030,0> translate<27.725000,0.000000,9.962500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.875000,-1.535000,41.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.750000,-1.535000,41.750000>}
box{<0,0,-0.075000><0.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<26.875000,-1.535000,41.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.625000,-1.535000,44.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.812500,-1.535000,44.125000>}
box{<0,0,-0.075000><0.197642,0.035000,0.075000> rotate<0,-18.433732,0> translate<27.625000,-1.535000,44.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.687500,-1.535000,37.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.875000,-1.535000,37.812500>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,-44.997030,0> translate<27.687500,-1.535000,37.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.875000,0.000000,37.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.000000,0.000000,37.937500>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,-44.997030,0> translate<27.875000,0.000000,37.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.937500,0.000000,45.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.000000,0.000000,45.812500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<27.937500,0.000000,45.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,34.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.062500,-1.535000,34.125000>}
box{<0,0,-0.075000><4.525000,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,34.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.812500,0.000000,38.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.062500,0.000000,38.500000>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<26.812500,0.000000,38.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.125000,-1.535000,40.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.062500,-1.535000,40.500000>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<27.125000,-1.535000,40.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<24.562500,0.000000,39.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.067500,0.000000,35.620000>}
box{<0,0,-0.075000><4.956819,0.035000,0.075000> rotate<0,44.997030,0> translate<24.562500,0.000000,39.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.687500,0.000000,38.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.120000,0.000000,36.130000>}
box{<0,0,-0.075000><3.440074,0.035000,0.075000> rotate<0,44.997030,0> translate<25.687500,0.000000,38.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.062500,-1.535000,37.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.125000,-1.535000,37.125000>}
box{<0,0,-0.075000><3.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<25.062500,-1.535000,37.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.062500,0.000000,40.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.182500,0.000000,40.620000>}
box{<0,0,-0.075000><0.169706,0.035000,0.075000> rotate<0,-44.997030,0> translate<28.062500,0.000000,40.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.062500,0.000000,38.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.187500,0.000000,38.375000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<28.062500,0.000000,38.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<27.125000,0.000000,11.037500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.212500,0.000000,11.037500>}
box{<0,0,-0.140000><1.087500,0.035000,0.140000> rotate<0,0.000000,0> translate<27.125000,0.000000,11.037500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<26.287500,0.000000,27.525000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<28.237500,0.000000,27.525000>}
box{<0,0,-0.406400><1.950000,0.035000,0.406400> rotate<0,0.000000,0> translate<26.287500,0.000000,27.525000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<26.275000,0.000000,51.225000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<28.237500,0.000000,51.225000>}
box{<0,0,-0.406400><1.962500,0.035000,0.406400> rotate<0,0.000000,0> translate<26.275000,0.000000,51.225000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.437500,0.000000,24.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.250000,0.000000,24.562500>}
box{<0,0,-0.075000><0.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<27.437500,0.000000,24.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.812500,-1.535000,39.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.250000,-1.535000,39.062500>}
box{<0,0,-0.075000><2.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<25.812500,-1.535000,39.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.062500,-1.535000,34.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.312500,-1.535000,33.875000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<28.062500,-1.535000,34.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.062500,0.000000,45.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.312500,0.000000,44.687500>}
box{<0,0,-0.075000><1.767767,0.035000,0.075000> rotate<0,44.997030,0> translate<27.062500,0.000000,45.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.875000,-1.535000,49.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.312500,-1.535000,48.312500>}
box{<0,0,-0.075000><2.032932,0.035000,0.075000> rotate<0,44.997030,0> translate<26.875000,-1.535000,49.750000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<25.437500,0.000000,13.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.337500,0.000000,13.125000>}
box{<0,0,-0.140000><2.900000,0.035000,0.140000> rotate<0,0.000000,0> translate<25.437500,0.000000,13.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.062500,-1.535000,42.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.375000,-1.535000,42.937500>}
box{<0,0,-0.075000><1.312500,0.035000,0.075000> rotate<0,0.000000,0> translate<27.062500,-1.535000,42.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<25.062500,-1.535000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.437500,-1.535000,36.625000>}
box{<0,0,-0.075000><3.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<25.062500,-1.535000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.437500,0.000000,47.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.437500,0.000000,46.250000>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,44.997030,0> translate<27.437500,0.000000,47.250000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.212500,0.000000,11.037500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.500000,0.000000,10.750000>}
box{<0,0,-0.140000><0.406586,0.035000,0.140000> rotate<0,44.997030,0> translate<28.212500,0.000000,11.037500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<28.500000,0.000000,11.662500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<28.500000,0.000000,10.750000>}
box{<0,0,-0.152400><0.912500,0.035000,0.152400> rotate<0,-90.000000,0> translate<28.500000,0.000000,10.750000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.337500,0.000000,13.125000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.500000,0.000000,12.962500>}
box{<0,0,-0.140000><0.229810,0.035000,0.140000> rotate<0,44.997030,0> translate<28.337500,0.000000,13.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<25.092600,0.000000,13.912500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.500000,0.000000,13.912500>}
box{<0,0,-0.140000><3.407400,0.035000,0.140000> rotate<0,0.000000,0> translate<25.092600,0.000000,13.912500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<27.537500,0.000000,15.212500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<28.500000,0.000000,15.212500>}
box{<0,0,-0.152400><0.962500,0.035000,0.152400> rotate<0,0.000000,0> translate<27.537500,0.000000,15.212500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.437500,-1.535000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.500000,-1.535000,36.687500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<28.437500,-1.535000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.125000,-1.535000,37.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.500000,-1.535000,37.500000>}
box{<0,0,-0.075000><0.530330,0.035000,0.075000> rotate<0,-44.997030,0> translate<28.125000,-1.535000,37.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.500000,0.000000,12.962500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.525000,0.000000,12.937500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,44.997030,0> translate<28.500000,0.000000,12.962500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.500000,0.000000,13.912500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.525000,0.000000,13.937500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<28.500000,0.000000,13.912500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.500000,0.000000,36.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.562500,0.000000,36.625000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<28.500000,0.000000,36.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.250000,0.000000,39.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.562500,0.000000,38.750000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<28.250000,0.000000,39.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.625000,-1.535000,48.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.562500,-1.535000,46.812500>}
box{<0,0,-0.075000><2.740039,0.035000,0.075000> rotate<0,44.997030,0> translate<26.625000,-1.535000,48.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,45.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.625000,0.000000,43.875000>}
box{<0,0,-0.075000><2.298097,0.035000,0.075000> rotate<0,44.997030,0> translate<27.000000,0.000000,45.500000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<14.750000,-1.535000,23.750000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<28.687500,-1.535000,23.750000>}
box{<0,0,-0.406400><13.937500,0.035000,0.406400> rotate<0,0.000000,0> translate<14.750000,-1.535000,23.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.250000,0.000000,43.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.687500,0.000000,43.500000>}
box{<0,0,-0.075000><1.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<27.250000,0.000000,43.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.250000,0.000000,24.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.750000,0.000000,24.062500>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,44.997030,0> translate<28.250000,0.000000,24.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.375000,0.000000,39.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.750000,0.000000,39.937500>}
box{<0,0,-0.075000><1.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<27.375000,0.000000,39.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.275000,0.000000,25.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.812500,0.000000,25.500000>}
box{<0,0,-0.075000><0.537500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.275000,0.000000,25.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<23.537500,-1.535000,34.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.875000,-1.535000,34.625000>}
box{<0,0,-0.075000><5.337500,0.035000,0.075000> rotate<0,0.000000,0> translate<23.537500,-1.535000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.000000,0.000000,39.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.875000,0.000000,39.625000>}
box{<0,0,-0.075000><1.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<27.000000,0.000000,39.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.562500,-1.535000,46.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.875000,-1.535000,46.812500>}
box{<0,0,-0.075000><0.312500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.562500,-1.535000,46.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.312500,0.000000,33.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.937500,0.000000,33.250000>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,44.997030,0> translate<28.312500,0.000000,33.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.687500,0.000000,43.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.937500,0.000000,43.250000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<28.687500,0.000000,43.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.750000,0.000000,39.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.062500,0.000000,40.250000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,-44.997030,0> translate<28.750000,0.000000,39.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.875000,0.000000,46.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.062500,0.000000,46.625000>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,44.997030,0> translate<28.875000,0.000000,46.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<19.000000,-1.535000,32.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,-1.535000,32.625000>}
box{<0,0,-0.075000><10.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<19.000000,-1.535000,32.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.875000,0.000000,39.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,0.000000,39.875000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,-44.997030,0> translate<28.875000,0.000000,39.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.937500,-1.535000,41.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,-1.535000,41.125000>}
box{<0,0,-0.075000><2.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<26.937500,-1.535000,41.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.625000,0.000000,43.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,0.000000,43.875000>}
box{<0,0,-0.075000><0.500000,0.035000,0.075000> rotate<0,0.000000,0> translate<28.625000,0.000000,43.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.437500,0.000000,46.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,0.000000,46.250000>}
box{<0,0,-0.075000><0.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.437500,0.000000,46.250000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<29.187500,-1.535000,23.250000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<29.187500,-1.535000,20.562500>}
box{<0,0,-0.406400><2.687500,0.035000,0.406400> rotate<0,-90.000000,0> translate<29.187500,-1.535000,20.562500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<28.687500,-1.535000,23.750000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<29.187500,-1.535000,23.250000>}
box{<0,0,-0.406400><0.707107,0.035000,0.406400> rotate<0,44.997030,0> translate<28.687500,-1.535000,23.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.312500,-1.535000,39.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.187500,-1.535000,39.687500>}
box{<0,0,-0.075000><1.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<27.312500,-1.535000,39.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,-1.535000,41.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.187500,-1.535000,41.187500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<29.125000,-1.535000,41.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,0.000000,45.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.187500,0.000000,45.187500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<29.125000,0.000000,45.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,0.000000,46.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.250000,0.000000,46.125000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<29.125000,0.000000,46.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,0.000000,43.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.437500,0.000000,43.562500>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<29.125000,0.000000,43.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.812500,0.000000,25.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.500000,0.000000,24.812500>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,44.997030,0> translate<28.812500,0.000000,25.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.500000,-1.535000,37.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.500000,-1.535000,37.500000>}
box{<0,0,-0.075000><1.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<28.500000,-1.535000,37.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.312500,0.000000,48.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.500000,0.000000,47.125000>}
box{<0,0,-0.075000><1.679379,0.035000,0.075000> rotate<0,44.997030,0> translate<28.312500,0.000000,48.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.187500,-1.535000,39.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.562500,-1.535000,39.312500>}
box{<0,0,-0.075000><0.530330,0.035000,0.075000> rotate<0,44.997030,0> translate<29.187500,-1.535000,39.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.250000,-1.535000,39.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.562500,-1.535000,39.312500>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<29.250000,-1.535000,39.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.875000,-1.535000,34.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.625000,-1.535000,33.875000>}
box{<0,0,-0.075000><1.060660,0.035000,0.075000> rotate<0,44.997030,0> translate<28.875000,-1.535000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.500000,-1.535000,37.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.625000,-1.535000,37.375000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<29.500000,-1.535000,37.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.125000,-1.535000,42.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.625000,-1.535000,42.312500>}
box{<0,0,-0.075000><2.500000,0.035000,0.075000> rotate<0,0.000000,0> translate<27.125000,-1.535000,42.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.625000,-1.535000,42.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.687500,-1.535000,42.375000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<29.625000,-1.535000,42.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.562500,0.000000,39.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.745000,0.000000,39.130000>}
box{<0,0,-0.075000><0.258094,0.035000,0.075000> rotate<0,44.997030,0> translate<29.562500,0.000000,39.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.812500,-1.535000,44.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.750000,-1.535000,44.125000>}
box{<0,0,-0.075000><1.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<27.812500,-1.535000,44.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.750000,0.000000,44.110000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.750000,0.000000,44.125000>}
box{<0,0,-0.075000><0.015000,0.035000,0.075000> rotate<0,90.000000,0> translate<29.750000,0.000000,44.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.687500,0.000000,35.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.812500,0.000000,35.125000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,-44.997030,0> translate<29.687500,0.000000,35.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.750000,0.000000,41.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.812500,0.000000,41.750000>}
box{<0,0,-0.075000><2.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<27.750000,0.000000,41.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<29.812500,0.000000,48.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<29.812500,0.000000,49.625000>}
box{<0,0,-0.150000><1.625000,0.035000,0.150000> rotate<0,90.000000,0> translate<29.812500,0.000000,49.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.625000,0.000000,37.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.870000,0.000000,37.130000>}
box{<0,0,-0.075000><0.346482,0.035000,0.075000> rotate<0,44.997030,0> translate<29.625000,0.000000,37.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.625000,0.000000,33.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.875000,0.000000,33.625000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<29.625000,0.000000,33.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.875000,0.000000,34.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.875000,0.000000,34.437500>}
box{<0,0,-0.075000><3.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<26.875000,0.000000,34.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.812500,0.000000,41.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.932500,0.000000,41.630000>}
box{<0,0,-0.075000><0.169706,0.035000,0.075000> rotate<0,44.997030,0> translate<29.812500,0.000000,41.750000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.525000,0.000000,13.937500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<29.937500,0.000000,13.937500>}
box{<0,0,-0.140000><1.412500,0.035000,0.140000> rotate<0,0.000000,0> translate<28.525000,0.000000,13.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.000000,0.000000,37.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.937500,0.000000,37.937500>}
box{<0,0,-0.075000><1.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.000000,0.000000,37.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,0.000000,39.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.937500,0.000000,39.875000>}
box{<0,0,-0.075000><0.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.125000,0.000000,39.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.687500,0.000000,42.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.937500,0.000000,42.375000>}
box{<0,0,-0.075000><0.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<29.687500,0.000000,42.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.375000,0.000000,42.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.937500,0.000000,42.937500>}
box{<0,0,-0.075000><1.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.375000,0.000000,42.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.187500,0.000000,38.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.000000,0.000000,38.375000>}
box{<0,0,-0.075000><1.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.187500,0.000000,38.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.000000,0.000000,45.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.062500,0.000000,45.812500>}
box{<0,0,-0.075000><2.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.000000,0.000000,45.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<29.812500,0.000000,49.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.062500,0.000000,49.875000>}
box{<0,0,-0.150000><0.353553,0.035000,0.150000> rotate<0,-44.997030,0> translate<29.812500,0.000000,49.625000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<28.525000,0.000000,12.937500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<30.125000,0.000000,12.937500>}
box{<0,0,-0.140000><1.600000,0.035000,0.140000> rotate<0,0.000000,0> translate<28.525000,0.000000,12.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.562500,0.000000,38.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.125000,0.000000,38.750000>}
box{<0,0,-0.075000><1.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.562500,0.000000,38.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.062500,0.000000,40.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.125000,0.000000,40.250000>}
box{<0,0,-0.075000><1.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.062500,0.000000,40.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.937500,0.000000,39.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.172500,0.000000,39.640000>}
box{<0,0,-0.075000><0.332340,0.035000,0.075000> rotate<0,44.997030,0> translate<29.937500,0.000000,39.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.937500,0.000000,33.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.187500,0.000000,33.250000>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<28.937500,0.000000,33.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.875000,0.000000,34.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.187500,0.000000,34.125000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<29.875000,0.000000,34.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.937500,0.000000,42.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.187500,0.000000,42.125000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<29.937500,0.000000,42.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.937500,0.000000,43.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.187500,0.000000,43.250000>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<28.937500,0.000000,43.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<29.812500,0.000000,48.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.187500,0.000000,47.625000>}
box{<0,0,-0.150000><0.530330,0.035000,0.150000> rotate<0,44.997030,0> translate<29.812500,0.000000,48.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.125000,0.000000,38.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.245000,0.000000,38.630000>}
box{<0,0,-0.075000><0.169706,0.035000,0.075000> rotate<0,44.997030,0> translate<30.125000,0.000000,38.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.937500,0.000000,42.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.250000,0.000000,42.625000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<29.937500,0.000000,42.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.062500,0.000000,45.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.250000,0.000000,45.625000>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,44.997030,0> translate<30.062500,0.000000,45.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.937500,0.000000,37.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.255000,0.000000,37.620000>}
box{<0,0,-0.075000><0.449013,0.035000,0.075000> rotate<0,44.997030,0> translate<29.937500,0.000000,37.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.000000,0.000000,38.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.255000,0.000000,38.120000>}
box{<0,0,-0.075000><0.360624,0.035000,0.075000> rotate<0,44.997030,0> translate<30.000000,0.000000,38.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.125000,0.000000,40.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.255000,0.000000,40.120000>}
box{<0,0,-0.075000><0.183848,0.035000,0.075000> rotate<0,44.997030,0> translate<30.125000,0.000000,40.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.187500,0.000000,33.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.312500,0.000000,33.125000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<30.187500,0.000000,33.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.187500,0.000000,43.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.312500,0.000000,43.125000>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<30.187500,0.000000,43.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.437500,0.000000,43.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.312500,0.000000,43.562500>}
box{<0,0,-0.075000><0.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<29.437500,0.000000,43.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.312500,0.000000,43.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.375000,0.000000,43.625000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<30.312500,0.000000,43.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.062500,0.000000,49.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.375000,0.000000,49.875000>}
box{<0,0,-0.150000><0.312500,0.035000,0.150000> rotate<0,0.000000,0> translate<30.062500,0.000000,49.875000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<29.187500,-1.535000,20.562500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<30.437500,-1.535000,19.312500>}
box{<0,0,-0.406400><1.767767,0.035000,0.406400> rotate<0,44.997030,0> translate<29.187500,-1.535000,20.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.187500,0.000000,41.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.875000,0.000000,41.187500>}
box{<0,0,-0.075000><1.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.187500,0.000000,41.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.312500,0.000000,44.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.875000,0.000000,44.687500>}
box{<0,0,-0.075000><2.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.312500,0.000000,44.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.187500,0.000000,45.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.875000,0.000000,45.187500>}
box{<0,0,-0.075000><1.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.187500,0.000000,45.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.375000,0.000000,49.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.875000,0.000000,50.375000>}
box{<0,0,-0.150000><0.707107,0.035000,0.150000> rotate<0,-44.997030,0> translate<30.375000,0.000000,49.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.625000,0.000000,31.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,31.625000>}
box{<0,0,-0.075000><1.312500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.625000,0.000000,31.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.125000,0.000000,32.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,32.625000>}
box{<0,0,-0.075000><1.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.125000,0.000000,32.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,32.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,32.630000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,32.630000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.312500,0.000000,33.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,33.125000>}
box{<0,0,-0.075000><0.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.312500,0.000000,33.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,33.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,33.130000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,33.130000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.875000,0.000000,33.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,33.625000>}
box{<0,0,-0.075000><1.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.875000,0.000000,33.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,33.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,33.630000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,33.630000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.187500,0.000000,34.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,34.125000>}
box{<0,0,-0.075000><0.750000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.187500,0.000000,34.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.812500,0.000000,35.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,35.125000>}
box{<0,0,-0.075000><1.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<29.812500,0.000000,35.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.067500,0.000000,35.620000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,35.620000>}
box{<0,0,-0.075000><2.870000,0.035000,0.075000> rotate<0,0.000000,0> translate<28.067500,0.000000,35.620000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,35.620000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,35.625000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,35.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,36.130000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,36.125000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,-90.000000,0> translate<30.937500,0.000000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.120000,0.000000,36.130000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,36.130000>}
box{<0,0,-0.075000><2.817500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.120000,0.000000,36.130000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.562500,0.000000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,36.625000>}
box{<0,0,-0.075000><2.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<28.562500,0.000000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,37.130000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,37.125000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,-90.000000,0> translate<30.937500,0.000000,37.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.870000,0.000000,37.130000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,37.130000>}
box{<0,0,-0.075000><1.067500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.870000,0.000000,37.130000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.255000,0.000000,37.620000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,37.620000>}
box{<0,0,-0.075000><0.682500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.255000,0.000000,37.620000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,37.620000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,37.625000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,37.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.255000,0.000000,38.120000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,38.120000>}
box{<0,0,-0.075000><0.682500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.255000,0.000000,38.120000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,38.120000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,38.125000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,38.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,38.630000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,38.625000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,-90.000000,0> translate<30.937500,0.000000,38.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.245000,0.000000,38.630000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,38.630000>}
box{<0,0,-0.075000><0.692500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.245000,0.000000,38.630000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,39.130000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,39.125000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,-90.000000,0> translate<30.937500,0.000000,39.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.745000,0.000000,39.130000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,39.130000>}
box{<0,0,-0.075000><1.192500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.745000,0.000000,39.130000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,39.640000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,39.625000>}
box{<0,0,-0.075000><0.015000,0.035000,0.075000> rotate<0,-90.000000,0> translate<30.937500,0.000000,39.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.172500,0.000000,39.640000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,39.640000>}
box{<0,0,-0.075000><0.765000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.172500,0.000000,39.640000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.255000,0.000000,40.120000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,40.120000>}
box{<0,0,-0.075000><0.682500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.255000,0.000000,40.120000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,40.120000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,40.125000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,40.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.182500,0.000000,40.620000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,40.620000>}
box{<0,0,-0.075000><2.755000,0.035000,0.075000> rotate<0,0.000000,0> translate<28.182500,0.000000,40.620000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,40.620000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,40.625000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,40.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.875000,0.000000,41.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,41.125000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<30.875000,0.000000,41.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,41.130000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,41.125000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,-90.000000,0> translate<30.937500,0.000000,41.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,41.630000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,41.625000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,-90.000000,0> translate<30.937500,0.000000,41.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.932500,0.000000,41.630000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,41.630000>}
box{<0,0,-0.075000><1.005000,0.035000,0.075000> rotate<0,0.000000,0> translate<29.932500,0.000000,41.630000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.187500,0.000000,42.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,42.125000>}
box{<0,0,-0.075000><0.750000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.187500,0.000000,42.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,42.130000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,42.125000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,-90.000000,0> translate<30.937500,0.000000,42.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.250000,0.000000,42.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,42.625000>}
box{<0,0,-0.075000><0.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.250000,0.000000,42.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,42.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,42.630000>}
box{<0,0,-0.075000><0.005000,0.035000,0.075000> rotate<0,90.000000,0> translate<30.937500,0.000000,42.630000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.312500,0.000000,43.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,43.125000>}
box{<0,0,-0.075000><0.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.312500,0.000000,43.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.375000,0.000000,43.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,43.625000>}
box{<0,0,-0.075000><0.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.375000,0.000000,43.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.750000,0.000000,44.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,44.125000>}
box{<0,0,-0.075000><1.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.750000,0.000000,44.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.875000,0.000000,44.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,44.625000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<30.875000,0.000000,44.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.875000,0.000000,45.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,45.125000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<30.875000,0.000000,45.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.250000,0.000000,45.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,45.625000>}
box{<0,0,-0.075000><0.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.250000,0.000000,45.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.250000,0.000000,46.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,46.125000>}
box{<0,0,-0.075000><1.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.250000,0.000000,46.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.062500,0.000000,46.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,46.625000>}
box{<0,0,-0.075000><1.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<29.062500,0.000000,46.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<29.500000,0.000000,47.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,47.125000>}
box{<0,0,-0.075000><1.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<29.500000,0.000000,47.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.187500,0.000000,47.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.937500,0.000000,47.625000>}
box{<0,0,-0.150000><0.750000,0.035000,0.150000> rotate<0,0.000000,0> translate<30.187500,0.000000,47.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,38.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.000000,0.000000,38.062500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<30.937500,0.000000,38.125000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<29.937500,0.000000,13.937500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<31.187500,0.000000,15.187500>}
box{<0,0,-0.140000><1.767767,0.035000,0.140000> rotate<0,-44.997030,0> translate<29.937500,0.000000,13.937500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<31.187500,0.000000,15.187500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<31.187500,0.000000,16.701000>}
box{<0,0,-0.140000><1.513500,0.035000,0.140000> rotate<0,90.000000,0> translate<31.187500,0.000000,16.701000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<28.750000,0.000000,24.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.332500,0.000000,24.062500>}
box{<0,0,-0.075000><2.582500,0.035000,0.075000> rotate<0,0.000000,0> translate<28.750000,0.000000,24.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.332500,0.000000,24.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.375000,0.000000,24.020000>}
box{<0,0,-0.075000><0.060104,0.035000,0.075000> rotate<0,44.997030,0> translate<31.332500,0.000000,24.062500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<30.125000,0.000000,12.937500>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<31.687500,0.000000,11.375000>}
box{<0,0,-0.140000><2.209709,0.035000,0.140000> rotate<0,44.997030,0> translate<30.125000,0.000000,12.937500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<31.687500,0.000000,7.549000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<31.687500,0.000000,11.375000>}
box{<0,0,-0.140000><3.826000,0.035000,0.140000> rotate<0,90.000000,0> translate<31.687500,0.000000,11.375000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<31.187500,0.000000,16.701000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<31.687500,0.000000,17.201000>}
box{<0,0,-0.140000><0.707107,0.035000,0.140000> rotate<0,-44.997030,0> translate<31.187500,0.000000,16.701000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<26.980000,-1.535000,3.957500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.750000,-1.535000,3.957500>}
box{<0,0,-0.075000><4.770000,0.035000,0.075000> rotate<0,0.000000,0> translate<26.980000,-1.535000,3.957500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,32.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.875000,0.000000,32.625000>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,32.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.937500,0.000000,48.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<31.875000,0.000000,48.125000>}
box{<0,0,-0.150000><0.937500,0.035000,0.150000> rotate<0,0.000000,0> translate<30.937500,0.000000,48.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.937500,0.000000,49.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<31.875000,0.000000,49.125000>}
box{<0,0,-0.150000><0.937500,0.035000,0.150000> rotate<0,0.000000,0> translate<30.937500,0.000000,49.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.925000,0.000000,25.575000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.925000,0.000000,26.512500>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,90.000000,0> translate<31.925000,0.000000,26.512500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.925000,0.000000,28.112500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,28.125000>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,-44.997030,0> translate<31.925000,0.000000,28.112500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,29.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,28.125000>}
box{<0,0,-0.075000><1.125000,0.035000,0.075000> rotate<0,-90.000000,0> translate<31.937500,0.000000,28.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,43.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,43.125000>}
box{<0,0,-0.075000><1.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,43.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,43.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,43.625000>}
box{<0,0,-0.075000><1.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,43.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,44.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,44.125000>}
box{<0,0,-0.075000><1.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,44.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,35.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.000000,0.000000,35.625000>}
box{<0,0,-0.075000><1.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,35.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,42.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.000000,0.000000,42.625000>}
box{<0,0,-0.075000><1.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,42.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,44.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.000000,0.000000,44.625000>}
box{<0,0,-0.075000><1.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,44.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<31.875000,0.000000,48.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.000000,0.000000,48.000000>}
box{<0,0,-0.150000><0.176777,0.035000,0.150000> rotate<0,44.997030,0> translate<31.875000,0.000000,48.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<31.875000,0.000000,49.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.000000,0.000000,49.250000>}
box{<0,0,-0.150000><0.176777,0.035000,0.150000> rotate<0,-44.997030,0> translate<31.875000,0.000000,49.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.062500,-1.535000,32.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.062500,-1.535000,32.000000>}
box{<0,0,-0.075000><0.437500,0.035000,0.075000> rotate<0,-90.000000,0> translate<32.062500,-1.535000,32.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.875000,0.000000,32.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.062500,0.000000,32.437500>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,44.997030,0> translate<31.875000,0.000000,32.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,42.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.062500,0.000000,42.125000>}
box{<0,0,-0.075000><1.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,42.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,45.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.062500,0.000000,45.125000>}
box{<0,0,-0.075000><1.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,45.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,45.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.125000,0.000000,45.625000>}
box{<0,0,-0.075000><1.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,45.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.750000,-1.535000,3.957500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.167500,-1.535000,3.540000>}
box{<0,0,-0.075000><0.590434,0.035000,0.075000> rotate<0,44.997030,0> translate<31.750000,-1.535000,3.957500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.167500,-1.535000,2.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.167500,-1.535000,3.540000>}
box{<0,0,-0.075000><1.165000,0.035000,0.075000> rotate<0,90.000000,0> translate<32.167500,-1.535000,3.540000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,36.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.187500,0.000000,36.125000>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,41.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.187500,0.000000,41.625000>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,41.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,46.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.187500,0.000000,46.125000>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,46.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,46.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.250000,0.000000,46.625000>}
box{<0,0,-0.075000><1.312500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,46.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,33.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.312500,0.000000,33.125000>}
box{<0,0,-0.075000><1.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,33.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,33.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.312500,0.000000,33.625000>}
box{<0,0,-0.075000><1.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,33.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,47.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.312500,0.000000,47.125000>}
box{<0,0,-0.075000><1.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,47.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.925000,0.000000,25.575000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.375000,0.000000,25.125000>}
box{<0,0,-0.075000><0.636396,0.035000,0.075000> rotate<0,44.997030,0> translate<31.925000,0.000000,25.575000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,34.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.375000,0.000000,34.625000>}
box{<0,0,-0.075000><1.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.375000,0.000000,36.625000>}
box{<0,0,-0.075000><1.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,37.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.375000,0.000000,37.125000>}
box{<0,0,-0.075000><1.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,37.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,37.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.437500,0.000000,37.625000>}
box{<0,0,-0.075000><1.500000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,37.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,38.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.500000,0.000000,38.625000>}
box{<0,0,-0.075000><1.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,38.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.000000,0.000000,38.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.562500,0.000000,38.062500>}
box{<0,0,-0.075000><1.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<31.000000,0.000000,38.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,39.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.562500,0.000000,39.125000>}
box{<0,0,-0.075000><1.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,39.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,41.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.562500,0.000000,41.125000>}
box{<0,0,-0.075000><1.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,41.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.937500,0.000000,48.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.562500,0.000000,48.625000>}
box{<0,0,-0.150000><1.625000,0.035000,0.150000> rotate<0,0.000000,0> translate<30.937500,0.000000,48.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,29.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.625000,0.000000,29.937500>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,-44.997030,0> translate<31.937500,0.000000,29.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.625000,0.000000,32.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.625000,0.000000,29.937500>}
box{<0,0,-0.075000><2.875000,0.035000,0.075000> rotate<0,-90.000000,0> translate<32.625000,0.000000,29.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.312500,0.000000,33.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.625000,0.000000,32.812500>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<32.312500,0.000000,33.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.725000,0.000000,26.161034>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.725000,0.000000,26.512500>}
box{<0,0,-0.075000><0.351466,0.035000,0.075000> rotate<0,90.000000,0> translate<32.725000,0.000000,26.512500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<30.437500,-1.535000,19.312500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<32.750000,-1.535000,19.312500>}
box{<0,0,-0.406400><2.312500,0.035000,0.406400> rotate<0,0.000000,0> translate<30.437500,-1.535000,19.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.725000,0.000000,28.112500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.750000,0.000000,28.137500>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.725000,0.000000,28.112500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.750000,0.000000,28.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.750000,0.000000,28.137500>}
box{<0,0,-0.075000><0.675000,0.035000,0.075000> rotate<0,-90.000000,0> translate<32.750000,0.000000,28.137500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.562500,0.000000,41.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.750000,0.000000,41.312500>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.562500,0.000000,41.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.562500,0.000000,48.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.812500,0.000000,48.875000>}
box{<0,0,-0.150000><0.353553,0.035000,0.150000> rotate<0,-44.997030,0> translate<32.562500,0.000000,48.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.812500,-1.535000,48.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.812500,-1.535000,51.625000>}
box{<0,0,-0.150000><2.750000,0.035000,0.150000> rotate<0,90.000000,0> translate<32.812500,-1.535000,51.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<27.312500,-1.535000,4.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.875000,-1.535000,4.562500>}
box{<0,0,-0.075000><5.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<27.312500,-1.535000,4.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.750000,0.000000,28.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.937500,0.000000,29.000000>}
box{<0,0,-0.075000><0.265165,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.750000,0.000000,28.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.937500,0.000000,33.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.937500,0.000000,29.000000>}
box{<0,0,-0.075000><4.000000,0.035000,0.075000> rotate<0,-90.000000,0> translate<32.937500,0.000000,29.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.312500,0.000000,33.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.937500,0.000000,33.000000>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,44.997030,0> translate<32.312500,0.000000,33.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,39.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.937500,0.000000,39.625000>}
box{<0,0,-0.075000><2.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,39.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.187500,0.000000,41.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.937500,0.000000,42.375000>}
box{<0,0,-0.075000><1.060660,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.187500,0.000000,41.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.750000,-1.535000,3.957500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.980000,-1.535000,3.957500>}
box{<0,0,-0.075000><1.230000,0.035000,0.075000> rotate<0,0.000000,0> translate<31.750000,-1.535000,3.957500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.375000,0.000000,25.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.053928,0.000000,25.125000>}
box{<0,0,-0.075000><0.678928,0.035000,0.075000> rotate<0,0.000000,0> translate<32.375000,0.000000,25.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,40.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.125000,0.000000,40.125000>}
box{<0,0,-0.075000><2.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,40.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.062500,0.000000,42.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.125000,0.000000,43.187500>}
box{<0,0,-0.075000><1.502602,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.062500,0.000000,42.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.250000,0.000000,33.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.250000,0.000000,32.125000>}
box{<0,0,-0.075000><1.625000,0.035000,0.075000> rotate<0,-90.000000,0> translate<33.250000,0.000000,32.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.375000,0.000000,34.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.250000,0.000000,33.750000>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,44.997030,0> translate<32.375000,0.000000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<30.937500,0.000000,40.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.250000,0.000000,40.625000>}
box{<0,0,-0.075000><2.312500,0.035000,0.075000> rotate<0,0.000000,0> translate<30.937500,0.000000,40.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.250000,0.000000,40.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.350000,0.000000,40.525000>}
box{<0,0,-0.075000><0.141421,0.035000,0.075000> rotate<0,44.997030,0> translate<33.250000,0.000000,40.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.000000,0.000000,42.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.375000,0.000000,44.000000>}
box{<0,0,-0.075000><1.944544,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.000000,0.000000,42.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.562500,0.000000,39.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.437500,0.000000,38.250000>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,44.997030,0> translate<32.562500,0.000000,39.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.525000,0.000000,26.068141>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.525000,0.000000,26.512500>}
box{<0,0,-0.075000><0.444359,0.035000,0.075000> rotate<0,90.000000,0> translate<33.525000,0.000000,26.512500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.125000,0.000000,40.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.525000,0.000000,39.725000>}
box{<0,0,-0.075000><0.565685,0.035000,0.075000> rotate<0,44.997030,0> translate<33.125000,0.000000,40.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.875000,-1.535000,4.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.562500,-1.535000,5.250000>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.875000,-1.535000,4.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.525000,0.000000,28.112500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.562500,0.000000,28.150000>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,-44.997030,0> translate<33.525000,0.000000,28.112500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.562500,0.000000,30.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.562500,0.000000,28.150000>}
box{<0,0,-0.075000><2.350000,0.035000,0.075000> rotate<0,-90.000000,0> translate<33.562500,0.000000,28.150000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.062500,-1.535000,32.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.562500,-1.535000,30.500000>}
box{<0,0,-0.075000><2.121320,0.035000,0.075000> rotate<0,44.997030,0> translate<32.062500,-1.535000,32.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,43.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.562500,0.000000,44.750000>}
box{<0,0,-0.075000><2.298097,0.035000,0.075000> rotate<0,-44.997030,0> translate<31.937500,0.000000,43.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.312500,0.000000,47.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.625000,0.000000,48.437500>}
box{<0,0,-0.075000><1.856155,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.312500,0.000000,47.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.625000,0.000000,48.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.625000,0.000000,50.187500>}
box{<0,0,-0.075000><1.750000,0.035000,0.075000> rotate<0,90.000000,0> translate<33.625000,0.000000,50.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.937500,0.000000,39.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.637500,0.000000,38.925000>}
box{<0,0,-0.075000><0.989949,0.035000,0.075000> rotate<0,44.997030,0> translate<32.937500,0.000000,39.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.980000,-1.535000,3.957500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.647500,-1.535000,4.625000>}
box{<0,0,-0.075000><0.943988,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.980000,-1.535000,3.957500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.500000,0.000000,38.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.687500,0.000000,37.437500>}
box{<0,0,-0.075000><1.679379,0.035000,0.075000> rotate<0,44.997030,0> translate<32.500000,0.000000,38.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.812500,0.000000,33.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.812500,0.000000,33.365441>}
box{<0,0,-0.075000><0.447059,0.035000,0.075000> rotate<0,-90.000000,0> translate<33.812500,0.000000,33.365441> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.000000,0.000000,35.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.812500,0.000000,33.812500>}
box{<0,0,-0.075000><2.563262,0.035000,0.075000> rotate<0,44.997030,0> translate<32.000000,0.000000,35.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.250000,0.000000,46.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.937500,0.000000,48.312500>}
box{<0,0,-0.075000><2.386485,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.250000,0.000000,46.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.937500,0.000000,48.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.937500,0.000000,50.000000>}
box{<0,0,-0.075000><1.687500,0.035000,0.075000> rotate<0,90.000000,0> translate<33.937500,0.000000,50.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.562500,0.000000,38.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.000000,0.000000,36.625000>}
box{<0,0,-0.075000><2.032932,0.035000,0.075000> rotate<0,44.997030,0> translate<32.562500,0.000000,38.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.812500,0.000000,33.365441>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.085784,0.000000,33.092156>}
box{<0,0,-0.075000><0.386482,0.035000,0.075000> rotate<0,44.997030,0> translate<33.812500,0.000000,33.365441> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.187500,0.000000,34.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.187500,0.000000,34.121809>}
box{<0,0,-0.075000><0.003191,0.035000,0.075000> rotate<0,-90.000000,0> translate<34.187500,0.000000,34.121809> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.187500,0.000000,36.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.187500,0.000000,34.125000>}
box{<0,0,-0.075000><2.828427,0.035000,0.075000> rotate<0,44.997030,0> translate<32.187500,0.000000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.437500,0.000000,37.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.187500,0.000000,35.875000>}
box{<0,0,-0.075000><2.474874,0.035000,0.075000> rotate<0,44.997030,0> translate<32.437500,0.000000,37.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,43.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.250000,0.000000,45.937500>}
box{<0,0,-0.075000><3.270369,0.035000,0.075000> rotate<0,-44.997030,0> translate<31.937500,0.000000,43.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.187500,0.000000,46.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.250000,0.000000,48.187500>}
box{<0,0,-0.075000><2.916815,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.187500,0.000000,46.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.250000,0.000000,48.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.250000,0.000000,49.687500>}
box{<0,0,-0.075000><1.500000,0.035000,0.075000> rotate<0,90.000000,0> translate<34.250000,0.000000,49.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.312500,0.000000,31.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.312500,0.000000,28.125000>}
box{<0,0,-0.075000><2.937500,0.035000,0.075000> rotate<0,-90.000000,0> translate<34.312500,0.000000,28.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.250000,0.000000,32.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.312500,0.000000,31.062500>}
box{<0,0,-0.075000><1.502602,0.035000,0.075000> rotate<0,44.997030,0> translate<33.250000,0.000000,32.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.375000,0.000000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.312500,0.000000,34.687500>}
box{<0,0,-0.075000><2.740039,0.035000,0.075000> rotate<0,44.997030,0> translate<32.375000,0.000000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.312500,0.000000,28.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.325000,0.000000,28.112500>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,44.997030,0> translate<34.312500,0.000000,28.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.375000,0.000000,37.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.375000,0.000000,35.125000>}
box{<0,0,-0.075000><2.828427,0.035000,0.075000> rotate<0,44.997030,0> translate<32.375000,0.000000,37.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.562500,0.000000,44.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.437500,0.000000,44.750000>}
box{<0,0,-0.075000><0.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<33.562500,0.000000,44.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.937500,0.000000,42.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.462500,0.000000,42.375000>}
box{<0,0,-0.075000><1.525000,0.035000,0.075000> rotate<0,0.000000,0> translate<32.937500,0.000000,42.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.125000,0.000000,43.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.475000,0.000000,43.187500>}
box{<0,0,-0.075000><1.350000,0.035000,0.075000> rotate<0,0.000000,0> translate<33.125000,0.000000,43.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.375000,0.000000,44.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.487500,0.000000,44.000000>}
box{<0,0,-0.075000><1.112500,0.035000,0.075000> rotate<0,0.000000,0> translate<33.375000,0.000000,44.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.325000,0.000000,26.512500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.494856,0.000000,26.512500>}
box{<0,0,-0.075000><0.169856,0.035000,0.075000> rotate<0,0.000000,0> translate<34.325000,0.000000,26.512500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.750000,0.000000,41.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.500000,0.000000,41.312500>}
box{<0,0,-0.075000><1.750000,0.035000,0.075000> rotate<0,0.000000,0> translate<32.750000,0.000000,41.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.637500,0.000000,38.925000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.512500,0.000000,38.925000>}
box{<0,0,-0.075000><0.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<33.637500,0.000000,38.925000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.525000,0.000000,39.725000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.512500,0.000000,39.725000>}
box{<0,0,-0.075000><0.987500,0.035000,0.075000> rotate<0,0.000000,0> translate<33.525000,0.000000,39.725000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.350000,0.000000,40.525000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.512500,0.000000,40.525000>}
box{<0,0,-0.075000><1.162500,0.035000,0.075000> rotate<0,0.000000,0> translate<33.350000,0.000000,40.525000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.500000,0.000000,41.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.512500,0.000000,41.325000>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,-44.997030,0> translate<34.500000,0.000000,41.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.462500,0.000000,42.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.512500,0.000000,42.425000>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,-44.997030,0> translate<34.462500,0.000000,42.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.475000,0.000000,43.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.512500,0.000000,43.225000>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,-44.997030,0> translate<34.475000,0.000000,43.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.487500,0.000000,44.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.512500,0.000000,44.025000>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<34.487500,0.000000,44.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.437500,0.000000,44.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.512500,0.000000,44.825000>}
box{<0,0,-0.075000><0.106066,0.035000,0.075000> rotate<0,-44.997030,0> translate<34.437500,0.000000,44.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.053928,0.000000,25.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.557609,0.000000,23.621319>}
box{<0,0,-0.075000><2.126526,0.035000,0.075000> rotate<0,44.997030,0> translate<33.053928,0.000000,25.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<31.937500,0.000000,44.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.562500,0.000000,46.750000>}
box{<0,0,-0.075000><3.712311,0.035000,0.075000> rotate<0,-44.997030,0> translate<31.937500,0.000000,44.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.125000,0.000000,45.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.562500,0.000000,48.062500>}
box{<0,0,-0.075000><3.447146,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.125000,0.000000,45.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.562500,0.000000,48.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.562500,0.000000,49.000000>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,90.000000,0> translate<34.562500,0.000000,49.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.812500,-1.535000,51.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<34.575000,-1.535000,53.387500>}
box{<0,0,-0.150000><2.492551,0.035000,0.150000> rotate<0,-44.997030,0> translate<32.812500,-1.535000,51.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.187500,0.000000,34.121809>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.651469,0.000000,33.657841>}
box{<0,0,-0.075000><0.656151,0.035000,0.075000> rotate<0,44.997030,0> translate<34.187500,0.000000,34.121809> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.562500,-1.535000,5.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.687500,-1.535000,5.250000>}
box{<0,0,-0.075000><1.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<33.562500,-1.535000,5.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.687500,0.000000,5.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.707500,0.000000,5.230000>}
box{<0,0,-0.075000><0.028284,0.035000,0.075000> rotate<0,44.997030,0> translate<34.687500,0.000000,5.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.707500,0.000000,2.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.707500,0.000000,5.230000>}
box{<0,0,-0.075000><2.855000,0.035000,0.075000> rotate<0,90.000000,0> translate<34.707500,0.000000,5.230000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.187500,0.000000,35.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.750000,0.000000,35.875000>}
box{<0,0,-0.075000><0.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<34.187500,0.000000,35.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.312500,0.000000,34.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.753184,0.000000,34.687500>}
box{<0,0,-0.075000><0.440684,0.035000,0.075000> rotate<0,0.000000,0> translate<34.312500,0.000000,34.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.437500,0.000000,38.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.775000,0.000000,38.250000>}
box{<0,0,-0.075000><1.337500,0.035000,0.075000> rotate<0,0.000000,0> translate<33.437500,0.000000,38.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.687500,0.000000,37.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.787500,0.000000,37.437500>}
box{<0,0,-0.075000><1.100000,0.035000,0.075000> rotate<0,0.000000,0> translate<33.687500,0.000000,37.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.250000,0.000000,49.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.787500,0.000000,50.225000>}
box{<0,0,-0.075000><0.760140,0.035000,0.075000> rotate<0,-44.997030,0> translate<34.250000,0.000000,49.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.000000,0.000000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.800000,0.000000,36.625000>}
box{<0,0,-0.075000><0.800000,0.035000,0.075000> rotate<0,0.000000,0> translate<34.000000,0.000000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.750000,0.000000,35.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.825000,0.000000,35.800000>}
box{<0,0,-0.075000><0.106066,0.035000,0.075000> rotate<0,44.997030,0> translate<34.750000,0.000000,35.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.800000,0.000000,36.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.825000,0.000000,36.600000>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,44.997030,0> translate<34.800000,0.000000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.787500,0.000000,37.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.825000,0.000000,37.400000>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,44.997030,0> translate<34.787500,0.000000,37.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.775000,0.000000,38.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.825000,0.000000,38.200000>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,44.997030,0> translate<34.775000,0.000000,38.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.000000,0.000000,44.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.900000,0.000000,47.525000>}
box{<0,0,-0.075000><4.101219,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.000000,0.000000,44.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.725000,0.000000,26.161034>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.911163,0.000000,23.974872>}
box{<0,0,-0.075000><3.091701,0.035000,0.075000> rotate<0,44.997030,0> translate<32.725000,0.000000,26.161034> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.937500,0.000000,50.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.962500,0.000000,51.025000>}
box{<0,0,-0.075000><1.449569,0.035000,0.075000> rotate<0,-44.997030,0> translate<33.937500,0.000000,50.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.562500,0.000000,49.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.987500,0.000000,49.425000>}
box{<0,0,-0.075000><0.601041,0.035000,0.075000> rotate<0,-44.997030,0> translate<34.562500,0.000000,49.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.753184,0.000000,34.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.217156,0.000000,34.223528>}
box{<0,0,-0.075000><0.656155,0.035000,0.075000> rotate<0,44.997030,0> translate<34.753184,0.000000,34.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<32.062500,0.000000,45.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.262500,0.000000,48.325000>}
box{<0,0,-0.075000><4.525483,0.035000,0.075000> rotate<0,-44.997030,0> translate<32.062500,0.000000,45.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.625000,0.000000,50.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.262500,0.000000,51.825000>}
box{<0,0,-0.075000><2.315775,0.035000,0.075000> rotate<0,-44.997030,0> translate<33.625000,0.000000,50.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.525000,0.000000,26.068141>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.264716,0.000000,24.328425>}
box{<0,0,-0.075000><2.460329,0.035000,0.075000> rotate<0,44.997030,0> translate<33.525000,0.000000,26.068141> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<34.575000,-1.535000,53.387500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<35.375000,-1.535000,53.387500>}
box{<0,0,-0.150000><0.800000,0.035000,0.150000> rotate<0,0.000000,0> translate<34.575000,-1.535000,53.387500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<35.300000,-1.535000,53.462500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<35.375000,-1.535000,53.387500>}
box{<0,0,-0.150000><0.106066,0.035000,0.150000> rotate<0,44.997030,0> translate<35.300000,-1.535000,53.462500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.375000,0.000000,35.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.447053,0.000000,35.125000>}
box{<0,0,-0.075000><1.072053,0.035000,0.075000> rotate<0,0.000000,0> translate<34.375000,0.000000,35.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.562500,0.000000,46.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.612500,0.000000,46.750000>}
box{<0,0,-0.075000><1.050000,0.035000,0.075000> rotate<0,0.000000,0> translate<34.562500,0.000000,46.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.217156,0.000000,31.960784>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.625000,0.000000,31.552941>}
box{<0,0,-0.075000><0.576778,0.035000,0.075000> rotate<0,44.997030,0> translate<35.217156,0.000000,31.960784> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.625000,0.000000,29.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.625000,0.000000,31.552941>}
box{<0,0,-0.075000><1.740441,0.035000,0.075000> rotate<0,90.000000,0> translate<35.625000,0.000000,31.552941> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.250000,0.000000,45.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.625000,0.000000,45.937500>}
box{<0,0,-0.075000><1.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<34.250000,0.000000,45.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.618272,0.000000,24.681978>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.630522,0.000000,24.681978>}
box{<0,0,-0.075000><0.012250,0.035000,0.075000> rotate<0,0.000000,0> translate<35.618272,0.000000,24.681978> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.625000,0.000000,45.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,45.925000>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,44.997030,0> translate<35.625000,0.000000,45.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.612500,0.000000,46.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,46.725000>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,44.997030,0> translate<35.612500,0.000000,46.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.900000,0.000000,47.525000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,47.525000>}
box{<0,0,-0.075000><0.737500,0.035000,0.075000> rotate<0,0.000000,0> translate<34.900000,0.000000,47.525000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.600000,0.000000,47.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,47.525000>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,44.997030,0> translate<35.600000,0.000000,47.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.262500,0.000000,48.325000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,48.325000>}
box{<0,0,-0.075000><0.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<35.262500,0.000000,48.325000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.987500,0.000000,49.425000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,49.425000>}
box{<0,0,-0.075000><0.650000,0.035000,0.075000> rotate<0,0.000000,0> translate<34.987500,0.000000,49.425000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.787500,0.000000,50.225000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,50.225000>}
box{<0,0,-0.075000><0.850000,0.035000,0.075000> rotate<0,0.000000,0> translate<34.787500,0.000000,50.225000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.962500,0.000000,51.025000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,51.025000>}
box{<0,0,-0.075000><0.675000,0.035000,0.075000> rotate<0,0.000000,0> translate<34.962500,0.000000,51.025000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.262500,0.000000,51.825000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.637500,0.000000,51.825000>}
box{<0,0,-0.075000><0.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<35.262500,0.000000,51.825000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.447053,0.000000,35.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.782841,0.000000,34.789213>}
box{<0,0,-0.075000><0.474875,0.035000,0.075000> rotate<0,44.997030,0> translate<35.447053,0.000000,35.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.971825,0.000000,17.964466>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.971825,0.000000,17.971825>}
box{<0,0,-0.075000><0.007359,0.035000,0.075000> rotate<0,90.000000,0> translate<35.971825,0.000000,17.971825> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.494856,0.000000,26.512500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.971825,0.000000,25.035531>}
box{<0,0,-0.075000><2.088749,0.035000,0.075000> rotate<0,44.997030,0> translate<34.494856,0.000000,26.512500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.625000,0.000000,29.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.000000,0.000000,29.437500>}
box{<0,0,-0.075000><0.530330,0.035000,0.075000> rotate<0,44.997030,0> translate<35.625000,0.000000,29.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.000000,0.000000,28.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.000000,0.000000,29.437500>}
box{<0,0,-0.075000><0.500000,0.035000,0.075000> rotate<0,90.000000,0> translate<36.000000,0.000000,29.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<35.782841,0.000000,32.526469>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.062500,0.000000,32.246809>}
box{<0,0,-0.075000><0.395498,0.035000,0.075000> rotate<0,44.997030,0> translate<35.782841,0.000000,32.526469> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.062500,0.000000,31.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.062500,0.000000,32.246809>}
box{<0,0,-0.075000><1.184309,0.035000,0.075000> rotate<0,90.000000,0> translate<36.062500,0.000000,32.246809> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.000000,-1.535000,28.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.125000,-1.535000,28.812500>}
box{<0,0,-0.075000><0.176777,0.035000,0.075000> rotate<0,44.997030,0> translate<36.000000,-1.535000,28.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.112500,0.000000,41.325000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.125000,0.000000,41.312500>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,44.997030,0> translate<36.112500,0.000000,41.325000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.112500,0.000000,40.525000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.137500,0.000000,40.500000>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,44.997030,0> translate<36.112500,0.000000,40.525000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.062500,0.000000,31.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.375000,0.000000,30.750000>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,44.997030,0> translate<36.062500,0.000000,31.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.375000,0.000000,30.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.375000,0.000000,30.750000>}
box{<0,0,-0.075000><0.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<36.375000,0.000000,30.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<35.630522,0.000000,24.681978>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.500000,0.000000,23.812500>}
box{<0,0,-0.150000><1.229628,0.035000,0.150000> rotate<0,44.997030,0> translate<35.630522,0.000000,24.681978> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.348528,0.000000,33.092156>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.750000,0.000000,32.690684>}
box{<0,0,-0.075000><0.567767,0.035000,0.075000> rotate<0,44.997030,0> translate<36.348528,0.000000,33.092156> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.750000,0.000000,31.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.750000,0.000000,32.690684>}
box{<0,0,-0.075000><1.190684,0.035000,0.075000> rotate<0,90.000000,0> translate<36.750000,0.000000,32.690684> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<35.971825,0.000000,17.971825>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.812500,0.000000,18.812500>}
box{<0,0,-0.150000><1.188894,0.035000,0.150000> rotate<0,-44.997030,0> translate<35.971825,0.000000,17.971825> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.812500,0.000000,18.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.812500,0.000000,19.062500>}
box{<0,0,-0.150000><0.250000,0.035000,0.150000> rotate<0,90.000000,0> translate<36.812500,0.000000,19.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.812500,-1.535000,19.106738>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.812500,-1.535000,19.062500>}
box{<0,0,-0.150000><0.044238,0.035000,0.150000> rotate<0,-90.000000,0> translate<36.812500,-1.535000,19.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.914213,0.000000,33.657841>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.967159,0.000000,33.657841>}
box{<0,0,-0.075000><0.052947,0.035000,0.075000> rotate<0,0.000000,0> translate<36.914213,0.000000,33.657841> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.375000,-1.535000,30.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.000000,-1.535000,29.500000>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,44.997030,0> translate<36.375000,-1.535000,30.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.112500,0.000000,39.725000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.025000,0.000000,39.725000>}
box{<0,0,-0.075000><0.912500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.112500,0.000000,39.725000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.112500,0.000000,43.225000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.025000,0.000000,43.225000>}
box{<0,0,-0.075000><0.912500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.112500,0.000000,43.225000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.112500,0.000000,44.825000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.050000,0.000000,44.825000>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.112500,0.000000,44.825000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.025000,0.000000,39.725000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.062500,0.000000,39.687500>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,44.997030,0> translate<37.025000,0.000000,39.725000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.025000,0.000000,43.225000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.062500,0.000000,43.187500>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,44.997030,0> translate<37.025000,0.000000,43.225000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.050000,0.000000,44.825000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.062500,0.000000,44.812500>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,44.997030,0> translate<37.050000,0.000000,44.825000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.750000,-1.535000,31.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.125000,-1.535000,31.125000>}
box{<0,0,-0.075000><0.530330,0.035000,0.075000> rotate<0,44.997030,0> translate<36.750000,-1.535000,31.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.125000,0.000000,41.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.187500,0.000000,41.312500>}
box{<0,0,-0.075000><1.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.125000,0.000000,41.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.325378,0.000000,17.610913>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.214466,0.000000,18.500000>}
box{<0,0,-0.150000><1.257360,0.035000,0.150000> rotate<0,-44.997030,0> translate<36.325378,0.000000,17.610913> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.237500,0.000000,49.425000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.250000,0.000000,49.437500>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,-44.997030,0> translate<37.237500,0.000000,49.425000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.237500,0.000000,46.725000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.262500,0.000000,46.750000>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<37.237500,0.000000,46.725000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.237500,0.000000,50.225000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.262500,0.000000,50.250000>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,-44.997030,0> translate<37.237500,0.000000,50.225000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.500000,-1.535000,23.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.272122,-1.535000,23.040378>}
box{<0,0,-0.150000><1.091945,0.035000,0.150000> rotate<0,44.997030,0> translate<36.500000,-1.535000,23.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.237500,0.000000,47.525000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.275000,0.000000,47.562500>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,-44.997030,0> translate<37.237500,0.000000,47.525000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.237500,0.000000,51.025000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.275000,0.000000,51.062500>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,-44.997030,0> translate<37.237500,0.000000,51.025000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.325378,0.000000,25.389084>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.276963,0.000000,24.437500>}
box{<0,0,-0.150000><1.345744,0.035000,0.150000> rotate<0,44.997030,0> translate<36.325378,0.000000,25.389084> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.237500,0.000000,45.925000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.287500,0.000000,45.875000>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,44.997030,0> translate<37.237500,0.000000,45.925000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.237500,0.000000,48.325000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.287500,0.000000,48.375000>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,-44.997030,0> translate<37.237500,0.000000,48.325000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.272122,-1.535000,23.040378>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.290381,-1.535000,23.102878>}
box{<0,0,-0.150000><0.065113,0.035000,0.150000> rotate<0,-73.709412,0> translate<37.272122,-1.535000,23.040378> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.276963,0.000000,24.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.312500,0.000000,24.437500>}
box{<0,0,-0.150000><0.035537,0.035000,0.150000> rotate<0,0.000000,0> translate<37.276963,0.000000,24.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.312500,0.000000,28.644603>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.312500,0.000000,31.125000>}
box{<0,0,-0.075000><2.480397,0.035000,0.075000> rotate<0,90.000000,0> translate<37.312500,0.000000,31.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.237500,0.000000,51.825000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.312500,0.000000,51.750000>}
box{<0,0,-0.075000><0.106066,0.035000,0.075000> rotate<0,44.997030,0> translate<37.237500,0.000000,51.825000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.425000,0.000000,35.800000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.325000,0.000000,35.800000>}
box{<0,0,-0.075000><0.900000,0.035000,0.075000> rotate<0,0.000000,0> translate<36.425000,0.000000,35.800000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.425000,0.000000,36.600000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.337500,0.000000,36.600000>}
box{<0,0,-0.075000><0.912500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.425000,0.000000,36.600000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.425000,0.000000,38.200000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.362500,0.000000,38.200000>}
box{<0,0,-0.075000><0.937500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.425000,0.000000,38.200000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.312500,-1.535000,24.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.375000,-1.535000,24.375000>}
box{<0,0,-0.150000><0.088388,0.035000,0.150000> rotate<0,44.997030,0> translate<37.312500,-1.535000,24.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.337500,0.000000,36.600000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.375000,0.000000,36.562500>}
box{<0,0,-0.075000><0.053033,0.035000,0.075000> rotate<0,44.997030,0> translate<37.337500,0.000000,36.600000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.362500,0.000000,38.200000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.375000,0.000000,38.187500>}
box{<0,0,-0.075000><0.017678,0.035000,0.075000> rotate<0,44.997030,0> translate<37.362500,0.000000,38.200000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.214466,0.000000,18.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.437500,0.000000,18.500000>}
box{<0,0,-0.150000><0.223034,0.035000,0.150000> rotate<0,0.000000,0> translate<37.214466,0.000000,18.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.437500,0.000000,33.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.437500,0.000000,32.687500>}
box{<0,0,-0.075000><0.500000,0.035000,0.075000> rotate<0,-90.000000,0> translate<37.437500,0.000000,32.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.967159,0.000000,33.657841>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.437500,0.000000,33.187500>}
box{<0,0,-0.075000><0.665162,0.035000,0.075000> rotate<0,44.997030,0> translate<36.967159,0.000000,33.657841> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<36.812500,-1.535000,19.106738>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.477881,-1.535000,19.834619>}
box{<0,0,-0.150000><0.986176,0.035000,0.150000> rotate<0,-47.565355,0> translate<36.812500,-1.535000,19.106738> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.687500,0.000000,28.976709>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.687500,0.000000,31.000000>}
box{<0,0,-0.075000><2.023291,0.035000,0.075000> rotate<0,90.000000,0> translate<37.687500,0.000000,31.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.437500,0.000000,32.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.687500,0.000000,32.437500>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<37.437500,0.000000,32.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.375000,-1.535000,24.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.750000,-1.535000,24.375000>}
box{<0,0,-0.150000><0.375000,0.035000,0.150000> rotate<0,0.000000,0> translate<37.375000,-1.535000,24.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.739591,0.000000,26.803300>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.759200,0.000000,26.803300>}
box{<0,0,-0.075000><0.019609,0.035000,0.075000> rotate<0,0.000000,0> translate<37.739591,0.000000,26.803300> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.325000,0.000000,35.800000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.812500,0.000000,35.312500>}
box{<0,0,-0.075000><0.689429,0.035000,0.075000> rotate<0,44.997030,0> translate<37.325000,0.000000,35.800000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.287500,0.000000,45.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.875000,0.000000,45.875000>}
box{<0,0,-0.075000><0.587500,0.035000,0.075000> rotate<0,0.000000,0> translate<37.287500,0.000000,45.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.437500,-1.535000,18.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.044244,-1.535000,18.500000>}
box{<0,0,-0.150000><0.606744,0.035000,0.150000> rotate<0,0.000000,0> translate<37.437500,-1.535000,18.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.062500,0.000000,29.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.062500,0.000000,30.937500>}
box{<0,0,-0.075000><1.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<38.062500,0.000000,30.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.112500,0.000000,42.425000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.075000,0.000000,42.425000>}
box{<0,0,-0.075000><1.962500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.112500,0.000000,42.425000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.093144,0.000000,27.156853>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.093147,0.000000,27.156853>}
box{<0,0,-0.075000><0.000003,0.035000,0.075000> rotate<0,0.000000,0> translate<38.093144,0.000000,27.156853> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.112500,0.000000,44.025000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.100000,0.000000,44.025000>}
box{<0,0,-0.075000><1.987500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.112500,0.000000,44.025000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.075000,0.000000,42.425000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.125000,0.000000,42.375000>}
box{<0,0,-0.075000><0.070711,0.035000,0.075000> rotate<0,44.997030,0> translate<38.075000,0.000000,42.425000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.100000,0.000000,44.025000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.125000,0.000000,44.000000>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,44.997030,0> translate<38.100000,0.000000,44.025000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.750000,-1.535000,24.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.165381,-1.535000,23.959619>}
box{<0,0,-0.150000><0.587438,0.035000,0.150000> rotate<0,44.997030,0> translate<37.750000,-1.535000,24.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.165381,-1.535000,23.959619>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.209622,-1.535000,24.022119>}
box{<0,0,-0.150000><0.076573,0.035000,0.150000> rotate<0,-54.703621,0> translate<38.165381,-1.535000,23.959619> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.200000,-1.535000,53.462500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.225000,-1.535000,53.462500>}
box{<0,0,-0.150000><1.025000,0.035000,0.150000> rotate<0,0.000000,0> translate<37.200000,-1.535000,53.462500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.125000,-1.535000,31.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,-1.535000,31.125000>}
box{<0,0,-0.075000><1.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.125000,-1.535000,31.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.312500,0.000000,31.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,0.000000,32.062500>}
box{<0,0,-0.075000><1.325825,0.035000,0.075000> rotate<0,-44.997030,0> translate<37.312500,0.000000,31.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,0.000000,32.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,0.000000,34.750000>}
box{<0,0,-0.075000><2.687500,0.035000,0.075000> rotate<0,90.000000,0> translate<38.250000,0.000000,34.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.137500,0.000000,40.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,0.000000,40.500000>}
box{<0,0,-0.075000><2.112500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.137500,0.000000,40.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.262500,0.000000,46.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,0.000000,46.750000>}
box{<0,0,-0.075000><0.987500,0.035000,0.075000> rotate<0,0.000000,0> translate<37.262500,0.000000,46.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.225000,-1.535000,53.462500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.250000,-1.535000,53.437500>}
box{<0,0,-0.150000><0.035355,0.035000,0.150000> rotate<0,44.997030,0> translate<38.225000,-1.535000,53.462500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.112500,0.000000,38.925000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.300000,0.000000,38.925000>}
box{<0,0,-0.075000><2.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<36.112500,0.000000,38.925000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.425000,0.000000,37.400000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.350000,0.000000,37.400000>}
box{<0,0,-0.075000><1.925000,0.035000,0.075000> rotate<0,0.000000,0> translate<36.425000,0.000000,37.400000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.350000,0.000000,37.400000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.375000,0.000000,37.375000>}
box{<0,0,-0.075000><0.035355,0.035000,0.075000> rotate<0,44.997030,0> translate<38.350000,0.000000,37.400000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.300000,0.000000,38.925000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.375000,0.000000,39.000000>}
box{<0,0,-0.075000><0.106066,0.035000,0.075000> rotate<0,-44.997030,0> translate<38.300000,0.000000,38.925000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.044244,-1.535000,18.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.397122,-1.535000,18.915378>}
box{<0,0,-0.150000><0.545034,0.035000,0.150000> rotate<0,-49.647668,0> translate<38.044244,-1.535000,18.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.437500,0.000000,29.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.437500,0.000000,30.875000>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,90.000000,0> translate<38.437500,0.000000,30.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.375000,-1.535000,39.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.437500,-1.535000,38.937500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<38.375000,-1.535000,39.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.312500,0.000000,28.644603>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.446697,0.000000,27.510406>}
box{<0,0,-0.075000><1.603997,0.035000,0.075000> rotate<0,44.997030,0> translate<37.312500,0.000000,28.644603> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.562500,-1.535000,25.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.562500,-1.535000,25.187500>}
box{<0,0,-0.150000><0.562500,0.035000,0.150000> rotate<0,-90.000000,0> translate<38.562500,-1.535000,25.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.562500,0.000000,26.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.562500,0.000000,25.750000>}
box{<0,0,-0.150000><0.250000,0.035000,0.150000> rotate<0,-90.000000,0> translate<38.562500,0.000000,25.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<37.759200,0.000000,26.803300>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.562500,0.000000,26.000000>}
box{<0,0,-0.150000><1.136038,0.035000,0.150000> rotate<0,44.997030,0> translate<37.759200,0.000000,26.803300> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.687500,0.000000,31.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.562500,0.000000,31.875000>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,-44.997030,0> translate<37.687500,0.000000,31.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.562500,0.000000,31.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.562500,0.000000,34.625000>}
box{<0,0,-0.075000><2.750000,0.035000,0.075000> rotate<0,90.000000,0> translate<38.562500,0.000000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.275000,0.000000,47.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.562500,0.000000,47.562500>}
box{<0,0,-0.075000><1.287500,0.035000,0.075000> rotate<0,0.000000,0> translate<37.275000,0.000000,47.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.687500,0.000000,28.976709>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.800250,0.000000,27.863959>}
box{<0,0,-0.075000><1.573666,0.035000,0.075000> rotate<0,44.997030,0> translate<37.687500,0.000000,28.976709> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.812500,0.000000,30.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.812500,0.000000,30.812500>}
box{<0,0,-0.075000><0.812500,0.035000,0.075000> rotate<0,90.000000,0> translate<38.812500,0.000000,30.812500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<36.250000,-1.535000,55.662500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<38.862500,-1.535000,55.662500>}
box{<0,0,-0.406400><2.612500,0.035000,0.406400> rotate<0,0.000000,0> translate<36.250000,-1.535000,55.662500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.093144,0.000000,27.156853>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.874997,0.000000,26.375000>}
box{<0,0,-0.150000><1.105707,0.035000,0.150000> rotate<0,44.997030,0> translate<38.093144,0.000000,27.156853> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.287500,0.000000,48.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.875000,0.000000,48.375000>}
box{<0,0,-0.075000><1.587500,0.035000,0.075000> rotate<0,0.000000,0> translate<37.287500,0.000000,48.375000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<38.862500,-1.535000,55.662500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<38.900000,-1.535000,55.625000>}
box{<0,0,-0.406400><0.053033,0.035000,0.406400> rotate<0,44.997030,0> translate<38.862500,-1.535000,55.662500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.062500,0.000000,30.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.937500,0.000000,31.812500>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,-44.997030,0> translate<38.062500,0.000000,30.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.937500,0.000000,31.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.937500,0.000000,34.500000>}
box{<0,0,-0.075000><2.687500,0.035000,0.075000> rotate<0,90.000000,0> translate<38.937500,0.000000,34.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,0.000000,34.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.937500,0.000000,35.437500>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,-44.997030,0> translate<38.250000,0.000000,34.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.875000,0.000000,45.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.937500,0.000000,44.812500>}
box{<0,0,-0.075000><1.502602,0.035000,0.075000> rotate<0,44.997030,0> translate<37.875000,0.000000,45.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.937500,0.000000,35.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.937500,0.000000,44.812500>}
box{<0,0,-0.075000><9.375000,0.035000,0.075000> rotate<0,90.000000,0> translate<38.937500,0.000000,44.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<36.125000,-1.535000,28.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.000000,-1.535000,28.812500>}
box{<0,0,-0.075000><2.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<36.125000,-1.535000,28.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.562500,-1.535000,25.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<39.040381,-1.535000,24.709619>}
box{<0,0,-0.150000><0.675826,0.035000,0.150000> rotate<0,44.997030,0> translate<38.562500,-1.535000,25.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.915381,-1.535000,24.790378>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<39.040381,-1.535000,24.709619>}
box{<0,0,-0.150000><0.148819,0.035000,0.150000> rotate<0,32.863323,0> translate<38.915381,-1.535000,24.790378> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<32.750000,-1.535000,19.312500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<39.062500,-1.535000,13.000000>}
box{<0,0,-0.406400><8.927223,0.035000,0.406400> rotate<0,44.997030,0> translate<32.750000,-1.535000,19.312500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<39.062500,-1.535000,8.562500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<39.062500,-1.535000,13.000000>}
box{<0,0,-0.406400><4.437500,0.035000,0.406400> rotate<0,90.000000,0> translate<39.062500,-1.535000,13.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<38.874997,0.000000,26.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<39.125000,0.000000,26.375000>}
box{<0,0,-0.150000><0.250003,0.035000,0.150000> rotate<0,0.000000,0> translate<38.874997,0.000000,26.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.000000,-1.535000,29.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.125000,-1.535000,29.500000>}
box{<0,0,-0.075000><2.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.000000,-1.535000,29.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.062500,0.000000,29.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.153803,0.000000,28.221197>}
box{<0,0,-0.075000><1.543336,0.035000,0.075000> rotate<0,44.997030,0> translate<38.062500,0.000000,29.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.153803,0.000000,28.217513>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.153803,0.000000,28.221197>}
box{<0,0,-0.075000><0.003684,0.035000,0.075000> rotate<0,90.000000,0> translate<39.153803,0.000000,28.221197> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,30.242638>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,30.812500>}
box{<0,0,-0.075000><0.569862,0.035000,0.075000> rotate<0,90.000000,0> translate<39.250000,0.000000,30.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.437500,0.000000,30.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,31.687500>}
box{<0,0,-0.075000><1.149049,0.035000,0.075000> rotate<0,-44.997030,0> translate<38.437500,0.000000,30.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,31.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,34.312500>}
box{<0,0,-0.075000><2.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<39.250000,0.000000,34.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.562500,0.000000,34.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,35.312500>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,-44.997030,0> translate<38.562500,0.000000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,0.000000,46.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,45.750000>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,44.997030,0> translate<38.250000,0.000000,46.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,35.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,45.750000>}
box{<0,0,-0.075000><10.437500,0.035000,0.075000> rotate<0,90.000000,0> translate<39.250000,0.000000,45.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.262500,0.000000,50.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.312500,0.000000,50.250000>}
box{<0,0,-0.075000><2.050000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.262500,0.000000,50.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.250000,0.000000,49.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.375000,0.000000,49.437500>}
box{<0,0,-0.075000><2.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.250000,0.000000,49.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.275000,0.000000,51.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.437500,0.000000,51.062500>}
box{<0,0,-0.075000><2.162500,0.035000,0.075000> rotate<0,0.000000,0> translate<37.275000,0.000000,51.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.437500,0.000000,29.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.491434,0.000000,28.571066>}
box{<0,0,-0.075000><1.490488,0.035000,0.075000> rotate<0,44.997030,0> translate<38.437500,0.000000,29.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.312500,0.000000,51.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.500000,0.000000,51.750000>}
box{<0,0,-0.075000><2.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<37.312500,0.000000,51.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.507356,0.000000,14.428931>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.507356,0.000000,14.444856>}
box{<0,0,-0.075000><0.015925,0.035000,0.075000> rotate<0,90.000000,0> translate<39.507356,0.000000,14.444856> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.491434,0.000000,28.571066>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.507356,0.000000,28.571066>}
box{<0,0,-0.075000><0.015922,0.035000,0.075000> rotate<0,0.000000,0> translate<39.491434,0.000000,28.571066> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.812500,0.000000,30.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.562500,0.000000,31.562500>}
box{<0,0,-0.075000><1.060660,0.035000,0.075000> rotate<0,-44.997030,0> translate<38.812500,0.000000,30.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.562500,0.000000,31.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.562500,0.000000,34.187500>}
box{<0,0,-0.075000><2.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<39.562500,0.000000,34.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.937500,0.000000,34.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.562500,0.000000,35.125000>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,-44.997030,0> translate<38.937500,0.000000,34.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.562500,0.000000,47.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.562500,0.000000,46.562500>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,44.997030,0> translate<38.562500,0.000000,47.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.562500,0.000000,35.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.562500,0.000000,46.562500>}
box{<0,0,-0.075000><11.437500,0.035000,0.075000> rotate<0,90.000000,0> translate<39.562500,0.000000,46.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.687500,0.000000,30.512244>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.687500,0.000000,30.812500>}
box{<0,0,-0.075000><0.300256,0.035000,0.075000> rotate<0,90.000000,0> translate<39.687500,0.000000,30.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.812500,0.000000,30.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.860913,0.000000,28.951588>}
box{<0,0,-0.075000><1.482679,0.035000,0.075000> rotate<0,44.997030,0> translate<38.812500,0.000000,30.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.860913,0.000000,28.924619>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.860913,0.000000,28.951588>}
box{<0,0,-0.075000><0.026969,0.035000,0.075000> rotate<0,90.000000,0> translate<39.860913,0.000000,28.951588> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,30.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.875000,0.000000,31.437500>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,-44.997030,0> translate<39.250000,0.000000,30.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.875000,0.000000,31.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.875000,0.000000,34.062500>}
box{<0,0,-0.075000><2.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<39.875000,0.000000,34.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,34.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.875000,0.000000,34.937500>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,-44.997030,0> translate<39.250000,0.000000,34.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.875000,0.000000,48.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.875000,0.000000,47.375000>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,44.997030,0> translate<38.875000,0.000000,48.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.875000,0.000000,34.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.875000,0.000000,47.375000>}
box{<0,0,-0.075000><12.437500,0.035000,0.075000> rotate<0,90.000000,0> translate<39.875000,0.000000,47.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.860913,0.000000,14.075378>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.887878,0.000000,14.075378>}
box{<0,0,-0.075000><0.026966,0.035000,0.075000> rotate<0,0.000000,0> translate<39.860913,0.000000,14.075378> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<39.125000,-1.535000,26.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<39.915381,-1.535000,25.584619>}
box{<0,0,-0.150000><1.117768,0.035000,0.150000> rotate<0,44.997030,0> translate<39.125000,-1.535000,26.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<39.834622,-1.535000,25.709619>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<39.915381,-1.535000,25.584619>}
box{<0,0,-0.150000><0.148819,0.035000,0.150000> rotate<0,57.130737,0> translate<39.834622,-1.535000,25.709619> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,30.719353>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,30.875000>}
box{<0,0,-0.075000><0.155647,0.035000,0.075000> rotate<0,90.000000,0> translate<40.187500,0.000000,30.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.687500,0.000000,30.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,31.312500>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,-44.997030,0> translate<39.687500,0.000000,30.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,31.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,33.937500>}
box{<0,0,-0.075000><2.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<40.187500,0.000000,33.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.562500,0.000000,34.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,34.812500>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,-44.997030,0> translate<39.562500,0.000000,34.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.375000,0.000000,49.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,48.625000>}
box{<0,0,-0.075000><1.149049,0.035000,0.075000> rotate<0,44.997030,0> translate<39.375000,0.000000,49.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,34.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,48.625000>}
box{<0,0,-0.075000><13.812500,0.035000,0.075000> rotate<0,90.000000,0> translate<40.187500,0.000000,48.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.250000,0.000000,30.242638>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.214466,0.000000,29.278172>}
box{<0,0,-0.075000><1.363960,0.035000,0.075000> rotate<0,44.997030,0> translate<39.250000,0.000000,30.242638> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<39.507356,0.000000,14.444856>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<40.375000,0.000000,15.312500>}
box{<0,0,-0.152400><1.227034,0.035000,0.152400> rotate<0,-44.997030,0> translate<39.507356,0.000000,14.444856> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<40.375000,0.000000,15.312500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<40.375000,0.000000,15.562500>}
box{<0,0,-0.152400><0.250000,0.035000,0.152400> rotate<0,90.000000,0> translate<40.375000,0.000000,15.562500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<40.375000,-1.535000,15.562500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<40.393262,-1.535000,15.562500>}
box{<0,0,-0.152400><0.018262,0.035000,0.152400> rotate<0,0.000000,0> translate<40.375000,-1.535000,15.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,30.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.500000,0.000000,31.187500>}
box{<0,0,-0.075000><0.441942,0.035000,0.075000> rotate<0,-44.997030,0> translate<40.187500,0.000000,30.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.500000,0.000000,31.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.500000,0.000000,33.812500>}
box{<0,0,-0.075000><2.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<40.500000,0.000000,33.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.875000,0.000000,34.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.500000,0.000000,34.687500>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,-44.997030,0> translate<39.875000,0.000000,34.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.312500,0.000000,50.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.500000,0.000000,49.062500>}
box{<0,0,-0.075000><1.679379,0.035000,0.075000> rotate<0,44.997030,0> translate<39.312500,0.000000,50.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.500000,0.000000,34.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.500000,0.000000,49.062500>}
box{<0,0,-0.075000><14.375000,0.035000,0.075000> rotate<0,90.000000,0> translate<40.500000,0.000000,49.062500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.550000,-1.535000,53.507500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.562500,-1.535000,53.495000>}
box{<0,0,-0.406400><0.017678,0.035000,0.406400> rotate<0,44.997030,0> translate<40.550000,-1.535000,53.507500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.687500,0.000000,30.512244>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.568019,0.000000,29.631725>}
box{<0,0,-0.075000><1.245242,0.035000,0.075000> rotate<0,44.997030,0> translate<39.687500,0.000000,30.512244> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.550000,-1.535000,53.507500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.600000,-1.535000,53.557500>}
box{<0,0,-0.406400><0.070711,0.035000,0.406400> rotate<0,-44.997030,0> translate<40.550000,-1.535000,53.507500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.600000,-1.535000,55.625000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.600000,-1.535000,53.557500>}
box{<0,0,-0.406400><2.067500,0.035000,0.406400> rotate<0,-90.000000,0> translate<40.600000,-1.535000,53.557500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.575000,-1.535000,55.650000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.600000,-1.535000,55.625000>}
box{<0,0,-0.406400><0.035355,0.035000,0.406400> rotate<0,44.997030,0> translate<40.575000,-1.535000,55.650000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<33.647500,-1.535000,4.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.687500,-1.535000,4.625000>}
box{<0,0,-0.075000><7.040000,0.035000,0.075000> rotate<0,0.000000,0> translate<33.647500,-1.535000,4.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.375000,-1.535000,36.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.687500,-1.535000,36.562500>}
box{<0,0,-0.075000><3.312500,0.035000,0.075000> rotate<0,0.000000,0> translate<37.375000,-1.535000,36.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<34.687500,-1.535000,5.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.750000,-1.535000,5.250000>}
box{<0,0,-0.075000><6.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<34.687500,-1.535000,5.250000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<39.887878,0.000000,14.075378>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<40.750000,0.000000,14.937500>}
box{<0,0,-0.152400><1.219224,0.035000,0.152400> rotate<0,-44.997030,0> translate<39.887878,0.000000,14.075378> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,33.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.812500,0.000000,34.562500>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,-44.997030,0> translate<40.187500,0.000000,33.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.437500,0.000000,51.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.812500,0.000000,49.687500>}
box{<0,0,-0.075000><1.944544,0.035000,0.075000> rotate<0,44.997030,0> translate<39.437500,0.000000,51.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.812500,0.000000,34.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.812500,0.000000,49.687500>}
box{<0,0,-0.075000><15.125000,0.035000,0.075000> rotate<0,90.000000,0> translate<40.812500,0.000000,49.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.187500,0.000000,30.719353>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.921572,0.000000,29.985281>}
box{<0,0,-0.075000><1.038134,0.035000,0.075000> rotate<0,44.997030,0> translate<40.187500,0.000000,30.719353> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<40.750000,0.000000,14.937500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<41.000000,0.000000,14.937500>}
box{<0,0,-0.152400><0.250000,0.035000,0.152400> rotate<0,0.000000,0> translate<40.750000,0.000000,14.937500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<40.393262,-1.535000,15.562500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<41.040381,-1.535000,16.209619>}
box{<0,0,-0.152400><0.915164,0.035000,0.152400> rotate<0,-44.997030,0> translate<40.393262,-1.535000,15.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.500000,0.000000,33.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.125000,0.000000,34.437500>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,-44.997030,0> translate<40.500000,0.000000,33.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.687500,-1.535000,36.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.125000,-1.535000,36.125000>}
box{<0,0,-0.075000><0.618718,0.035000,0.075000> rotate<0,44.997030,0> translate<40.687500,-1.535000,36.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.500000,0.000000,51.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.125000,0.000000,50.125000>}
box{<0,0,-0.075000><2.298097,0.035000,0.075000> rotate<0,44.997030,0> translate<39.500000,0.000000,51.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.125000,0.000000,34.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.125000,0.000000,50.125000>}
box{<0,0,-0.075000><15.687500,0.035000,0.075000> rotate<0,90.000000,0> translate<41.125000,0.000000,50.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.375000,-1.535000,37.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.187500,-1.535000,37.375000>}
box{<0,0,-0.075000><2.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<38.375000,-1.535000,37.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.275125,0.000000,30.338834>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.286166,0.000000,30.338834>}
box{<0,0,-0.075000><0.011041,0.035000,0.075000> rotate<0,0.000000,0> translate<41.275125,0.000000,30.338834> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.312500,0.000000,31.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.312500,0.000000,33.375000>}
box{<0,0,-0.075000><1.687500,0.035000,0.075000> rotate<0,90.000000,0> translate<41.312500,0.000000,33.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.375000,-1.535000,38.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.437500,-1.535000,38.187500>}
box{<0,0,-0.075000><4.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<37.375000,-1.535000,38.187500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<41.000000,-1.535000,14.937500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<41.606744,-1.535000,14.937500>}
box{<0,0,-0.152400><0.606744,0.035000,0.152400> rotate<0,0.000000,0> translate<41.000000,-1.535000,14.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.187500,-1.535000,37.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.625000,-1.535000,36.937500>}
box{<0,0,-0.075000><0.618718,0.035000,0.075000> rotate<0,44.997030,0> translate<41.187500,-1.535000,37.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.628678,0.000000,30.692388>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.628678,0.000000,30.683822>}
box{<0,0,-0.075000><0.008566,0.035000,0.075000> rotate<0,-90.000000,0> translate<41.628678,0.000000,30.683822> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.000000,-1.535000,28.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.687500,-1.535000,26.125000>}
box{<0,0,-0.075000><3.800699,0.035000,0.075000> rotate<0,44.997030,0> translate<39.000000,-1.535000,28.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<39.125000,-1.535000,29.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.687500,-1.535000,26.937500>}
box{<0,0,-0.075000><3.623922,0.035000,0.075000> rotate<0,44.997030,0> translate<39.125000,-1.535000,29.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.437500,-1.535000,38.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.687500,-1.535000,38.937500>}
box{<0,0,-0.075000><3.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<38.437500,-1.535000,38.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.750000,0.000000,32.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.750000,0.000000,33.125000>}
box{<0,0,-0.075000><1.125000,0.035000,0.075000> rotate<0,90.000000,0> translate<41.750000,0.000000,33.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,-1.535000,31.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.812500,-1.535000,27.562500>}
box{<0,0,-0.075000><5.038136,0.035000,0.075000> rotate<0,44.997030,0> translate<38.250000,-1.535000,31.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.437500,-1.535000,38.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.875000,-1.535000,37.750000>}
box{<0,0,-0.075000><0.618718,0.035000,0.075000> rotate<0,44.997030,0> translate<41.437500,-1.535000,38.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.312500,0.000000,31.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.954059,0.000000,31.045941>}
box{<0,0,-0.075000><0.907302,0.035000,0.075000> rotate<0,44.997030,0> translate<41.312500,0.000000,31.687500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<41.606744,-1.535000,14.937500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<41.959622,-1.535000,15.290378>}
box{<0,0,-0.152400><0.499045,0.035000,0.152400> rotate<0,-44.997030,0> translate<41.606744,-1.535000,14.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.954059,0.000000,31.045941>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.982231,0.000000,31.045941>}
box{<0,0,-0.075000><0.028172,0.035000,0.075000> rotate<0,0.000000,0> translate<41.954059,0.000000,31.045941> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.062500,0.000000,29.551459>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.062500,0.000000,29.125000>}
box{<0,0,-0.150000><0.426459,0.035000,0.150000> rotate<0,-90.000000,0> translate<42.062500,0.000000,29.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<41.275125,0.000000,30.338834>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.062500,0.000000,29.551459>}
box{<0,0,-0.150000><1.113516,0.035000,0.150000> rotate<0,44.997030,0> translate<41.275125,0.000000,30.338834> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.687500,-1.535000,32.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.125000,-1.535000,28.000000>}
box{<0,0,-0.075000><6.275573,0.035000,0.075000> rotate<0,44.997030,0> translate<37.687500,-1.535000,32.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.062500,-1.535000,29.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.272122,-1.535000,28.915378>}
box{<0,0,-0.150000><0.296450,0.035000,0.150000> rotate<0,44.997030,0> translate<42.062500,-1.535000,29.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.312500,0.000000,33.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.312500,0.000000,34.375000>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,-44.997030,0> translate<41.312500,0.000000,33.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.687500,0.000000,38.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.312500,0.000000,38.312500>}
box{<0,0,-0.075000><0.883883,0.035000,0.075000> rotate<0,44.997030,0> translate<41.687500,0.000000,38.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.312500,0.000000,34.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.312500,0.000000,38.312500>}
box{<0,0,-0.075000><3.937500,0.035000,0.075000> rotate<0,90.000000,0> translate<42.312500,0.000000,38.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.750000,0.000000,32.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.335784,0.000000,31.414216>}
box{<0,0,-0.075000><0.828424,0.035000,0.075000> rotate<0,44.997030,0> translate<41.750000,0.000000,32.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.335784,0.000000,31.399494>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.335784,0.000000,31.414216>}
box{<0,0,-0.075000><0.014722,0.035000,0.075000> rotate<0,90.000000,0> translate<42.335784,0.000000,31.414216> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<41.628678,0.000000,30.692388>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.446066,0.000000,29.875000>}
box{<0,0,-0.150000><1.155960,0.035000,0.150000> rotate<0,44.997030,0> translate<41.628678,0.000000,30.692388> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.446066,0.000000,29.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.625000,0.000000,29.875000>}
box{<0,0,-0.150000><0.178934,0.035000,0.150000> rotate<0,0.000000,0> translate<42.446066,0.000000,29.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.625000,-1.535000,29.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.812500,-1.535000,29.875000>}
box{<0,0,-0.150000><0.187500,0.035000,0.150000> rotate<0,0.000000,0> translate<42.625000,-1.535000,29.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.062500,-1.535000,39.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.812500,-1.535000,39.687500>}
box{<0,0,-0.075000><5.750000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.062500,-1.535000,39.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.687500,-1.535000,4.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.937500,-1.535000,2.375000>}
box{<0,0,-0.075000><3.181981,0.035000,0.075000> rotate<0,44.997030,0> translate<40.687500,-1.535000,4.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<40.750000,-1.535000,5.250000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.062500,-1.535000,2.937500>}
box{<0,0,-0.075000><3.270369,0.035000,0.075000> rotate<0,44.997030,0> translate<40.750000,-1.535000,5.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.750000,0.000000,33.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.062500,0.000000,34.437500>}
box{<0,0,-0.075000><1.856155,0.035000,0.075000> rotate<0,-44.997030,0> translate<41.750000,0.000000,33.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.812500,0.000000,39.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.062500,0.000000,39.437500>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,44.997030,0> translate<42.812500,0.000000,39.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.062500,0.000000,34.437500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.062500,0.000000,39.437500>}
box{<0,0,-0.075000><5.000000,0.035000,0.075000> rotate<0,90.000000,0> translate<43.062500,0.000000,39.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.812500,-1.535000,29.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<43.187500,-1.535000,30.250000>}
box{<0,0,-0.150000><0.530330,0.035000,0.150000> rotate<0,-44.997030,0> translate<42.812500,-1.535000,29.875000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<39.062500,-1.535000,8.562500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<43.250000,-1.535000,4.375000>}
box{<0,0,-0.406400><5.922019,0.035000,0.406400> rotate<0,44.997030,0> translate<39.062500,-1.535000,8.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<42.272122,-1.535000,28.915378>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<43.290381,-1.535000,28.915378>}
box{<0,0,-0.150000><1.018259,0.035000,0.150000> rotate<0,0.000000,0> translate<42.272122,-1.535000,28.915378> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.687500,-1.535000,26.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.687500,-1.535000,26.125000>}
box{<0,0,-0.075000><2.000000,0.035000,0.075000> rotate<0,0.000000,0> translate<41.687500,-1.535000,26.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<43.187500,-1.535000,30.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<43.794241,-1.535000,30.250000>}
box{<0,0,-0.150000><0.606741,0.035000,0.150000> rotate<0,0.000000,0> translate<43.187500,-1.535000,30.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.687500,0.000000,26.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.875000,0.000000,26.125000>}
box{<0,0,-0.075000><0.187500,0.035000,0.075000> rotate<0,0.000000,0> translate<43.687500,0.000000,26.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.250000,-1.535000,40.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.875000,-1.535000,40.500000>}
box{<0,0,-0.075000><5.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<38.250000,-1.535000,40.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.687500,-1.535000,26.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.937500,-1.535000,26.937500>}
box{<0,0,-0.075000><2.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<41.687500,-1.535000,26.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.022122,-1.535000,10.415378>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.022122,-1.535000,10.459622>}
box{<0,0,-0.150000><0.044244,0.035000,0.150000> rotate<0,90.000000,0> translate<44.022122,-1.535000,10.459622> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<43.915381,-1.535000,13.915378>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.080759,-1.535000,13.750000>}
box{<0,0,-0.150000><0.233880,0.035000,0.150000> rotate<0,44.997030,0> translate<43.915381,-1.535000,13.915378> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<43.102881,-1.535000,11.334619>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.125000,-1.535000,12.356738>}
box{<0,0,-0.150000><1.445494,0.035000,0.150000> rotate<0,-44.997030,0> translate<43.102881,-1.535000,11.334619> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.125000,-1.535000,12.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.125000,-1.535000,12.356738>}
box{<0,0,-0.150000><0.330762,0.035000,0.150000> rotate<0,-90.000000,0> translate<44.125000,-1.535000,12.356738> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<43.794241,-1.535000,30.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.209622,-1.535000,29.834619>}
box{<0,0,-0.150000><0.587438,0.035000,0.150000> rotate<0,44.997030,0> translate<43.794241,-1.535000,30.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.062500,-1.535000,2.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.437500,-1.535000,2.937500>}
box{<0,0,-0.075000><1.375000,0.035000,0.075000> rotate<0,0.000000,0> translate<43.062500,-1.535000,2.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.125000,0.000000,12.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.562500,0.000000,13.125000>}
box{<0,0,-0.150000><0.618718,0.035000,0.150000> rotate<0,-44.997030,0> translate<44.125000,0.000000,12.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.022122,-1.535000,10.459622>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.687500,-1.535000,11.125000>}
box{<0,0,-0.150000><0.940987,0.035000,0.150000> rotate<0,-44.997030,0> translate<44.022122,-1.535000,10.459622> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.812500,-1.535000,27.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.687500,-1.535000,27.562500>}
box{<0,0,-0.075000><2.875000,0.035000,0.075000> rotate<0,0.000000,0> translate<41.812500,-1.535000,27.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.937500,-1.535000,2.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.750000,-1.535000,2.375000>}
box{<0,0,-0.075000><1.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<42.937500,-1.535000,2.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.937500,-1.535000,26.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.750000,-1.535000,26.125000>}
box{<0,0,-0.075000><1.149049,0.035000,0.075000> rotate<0,44.997030,0> translate<43.937500,-1.535000,26.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.834622,-1.535000,14.834619>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.852881,-1.535000,14.834619>}
box{<0,0,-0.150000><0.018259,0.035000,0.150000> rotate<0,0.000000,0> translate<44.834622,-1.535000,14.834619> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.750000,-1.535000,26.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.875000,-1.535000,26.125000>}
box{<0,0,-0.075000><0.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<44.750000,-1.535000,26.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.875000,0.000000,26.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.875000,0.000000,27.125000>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,-44.997030,0> translate<43.875000,0.000000,26.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.437500,-1.535000,2.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.937500,-1.535000,3.375000>}
box{<0,0,-0.075000><0.664384,0.035000,0.075000> rotate<0,-41.183207,0> translate<44.437500,-1.535000,2.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.187500,-1.535000,41.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.937500,-1.535000,41.312500>}
box{<0,0,-0.075000><7.750000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.187500,-1.535000,41.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.562500,0.000000,13.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.053928,0.000000,13.125000>}
box{<0,0,-0.150000><0.491428,0.035000,0.150000> rotate<0,0.000000,0> translate<44.562500,0.000000,13.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.080759,-1.535000,13.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.125000,-1.535000,13.750000>}
box{<0,0,-0.150000><1.044241,0.035000,0.150000> rotate<0,0.000000,0> translate<44.080759,-1.535000,13.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.875000,0.000000,26.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.125000,0.000000,26.125000>}
box{<0,0,-0.075000><0.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<44.875000,0.000000,26.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.687500,0.000000,11.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.163003,0.000000,11.600503>}
box{<0,0,-0.150000><0.672463,0.035000,0.150000> rotate<0,-44.997030,0> translate<44.687500,0.000000,11.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.163003,0.000000,11.600503>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.164213,0.000000,11.600503>}
box{<0,0,-0.150000><0.001209,0.035000,0.150000> rotate<0,0.000000,0> translate<45.163003,0.000000,11.600503> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.164213,0.000000,31.399494>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.164213,0.000000,31.414213>}
box{<0,0,-0.075000><0.014719,0.035000,0.075000> rotate<0,90.000000,0> translate<45.164213,0.000000,31.414213> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.750000,-1.535000,2.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.250000,-1.535000,2.875000>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,-44.997030,0> translate<44.750000,-1.535000,2.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<42.125000,-1.535000,28.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.250000,-1.535000,28.000000>}
box{<0,0,-0.075000><3.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<42.125000,-1.535000,28.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.852881,-1.535000,14.834619>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.312500,-1.535000,14.375000>}
box{<0,0,-0.150000><0.649999,0.035000,0.150000> rotate<0,44.997030,0> translate<44.852881,-1.535000,14.834619> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.125000,0.000000,26.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.687500,0.000000,26.687500>}
box{<0,0,-0.075000><0.795495,0.035000,0.075000> rotate<0,-44.997030,0> translate<45.125000,0.000000,26.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.125000,-1.535000,42.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.687500,-1.535000,42.375000>}
box{<0,0,-0.075000><7.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<38.125000,-1.535000,42.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.053928,0.000000,13.125000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.871319,0.000000,12.307609>}
box{<0,0,-0.150000><1.155965,0.035000,0.150000> rotate<0,44.997030,0> translate<45.053928,0.000000,13.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.871319,0.000000,12.307609>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.871319,0.000000,12.316181>}
box{<0,0,-0.075000><0.008572,0.035000,0.075000> rotate<0,90.000000,0> translate<45.871319,0.000000,12.316181> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<40.550000,-1.535000,53.507500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<45.932500,-1.535000,48.125000>}
box{<0,0,-0.406400><7.612004,0.035000,0.406400> rotate<0,44.997030,0> translate<40.550000,-1.535000,53.507500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.312500,-1.535000,14.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.937500,-1.535000,14.375000>}
box{<0,0,-0.150000><0.625000,0.035000,0.150000> rotate<0,0.000000,0> translate<45.312500,-1.535000,14.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.164213,0.000000,31.414213>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.937500,0.000000,32.187500>}
box{<0,0,-0.075000><1.093594,0.035000,0.075000> rotate<0,-44.997030,0> translate<45.164213,0.000000,31.414213> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<43.875000,0.000000,40.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.937500,0.000000,38.437500>}
box{<0,0,-0.075000><2.916815,0.035000,0.075000> rotate<0,44.997030,0> translate<43.875000,0.000000,40.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.937500,0.000000,32.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.937500,0.000000,38.437500>}
box{<0,0,-0.075000><6.250000,0.035000,0.075000> rotate<0,90.000000,0> translate<45.937500,0.000000,38.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.687500,-1.535000,27.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.187500,-1.535000,26.062500>}
box{<0,0,-0.075000><2.121320,0.035000,0.075000> rotate<0,44.997030,0> translate<44.687500,-1.535000,27.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.602881,-1.535000,15.602878>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.205759,-1.535000,15.000000>}
box{<0,0,-0.150000><0.852598,0.035000,0.150000> rotate<0,44.997030,0> translate<45.602881,-1.535000,15.602878> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.125000,0.000000,13.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.213838,0.000000,12.661163>}
box{<0,0,-0.150000><1.539849,0.035000,0.150000> rotate<0,44.997030,0> translate<45.125000,0.000000,13.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.213838,0.000000,12.661163>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.224872,0.000000,12.661163>}
box{<0,0,-0.150000><0.011034,0.035000,0.150000> rotate<0,0.000000,0> translate<46.213838,0.000000,12.661163> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.224872,0.000000,30.338834>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.224872,0.000000,30.349872>}
box{<0,0,-0.075000><0.011038,0.035000,0.075000> rotate<0,90.000000,0> translate<46.224872,0.000000,30.349872> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.187500,0.000000,26.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.250000,0.000000,26.000000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<46.187500,0.000000,26.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.517766,0.000000,31.045941>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.437500,0.000000,31.965675>}
box{<0,0,-0.075000><1.300701,0.035000,0.075000> rotate<0,-44.997030,0> translate<45.517766,0.000000,31.045941> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.937500,0.000000,41.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.437500,0.000000,39.812500>}
box{<0,0,-0.075000><2.121320,0.035000,0.075000> rotate<0,44.997030,0> translate<44.937500,0.000000,41.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.437500,0.000000,31.965675>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.437500,0.000000,39.812500>}
box{<0,0,-0.075000><7.846825,0.035000,0.075000> rotate<0,90.000000,0> translate<46.437500,0.000000,39.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.205759,-1.535000,15.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.500000,-1.535000,15.000000>}
box{<0,0,-0.150000><0.294241,0.035000,0.150000> rotate<0,0.000000,0> translate<46.205759,-1.535000,15.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.875000,0.000000,27.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.500000,0.000000,27.125000>}
box{<0,0,-0.075000><1.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<44.875000,0.000000,27.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.522122,-1.535000,16.477878>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.522122,-1.535000,16.522119>}
box{<0,0,-0.150000><0.044241,0.035000,0.150000> rotate<0,90.000000,0> translate<46.522122,-1.535000,16.522119> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.578425,0.000000,29.985281>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.578425,0.000000,30.015925>}
box{<0,0,-0.075000><0.030644,0.035000,0.075000> rotate<0,90.000000,0> translate<46.578425,0.000000,30.015925> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<45.937500,0.000000,14.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.632356,0.000000,14.375000>}
box{<0,0,-0.150000><0.694856,0.035000,0.150000> rotate<0,0.000000,0> translate<45.937500,0.000000,14.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.062500,-1.535000,43.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.687500,-1.535000,43.187500>}
box{<0,0,-0.075000><9.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.062500,-1.535000,43.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.500000,0.000000,15.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.714463,0.000000,15.000000>}
box{<0,0,-0.150000><0.214463,0.035000,0.150000> rotate<0,0.000000,0> translate<46.500000,0.000000,15.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.250000,0.000000,26.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.835788,0.000000,26.000000>}
box{<0,0,-0.075000><0.585788,0.035000,0.075000> rotate<0,0.000000,0> translate<46.250000,0.000000,26.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.522122,-1.535000,16.477878>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.875000,-1.535000,16.125000>}
box{<0,0,-0.150000><0.499045,0.035000,0.150000> rotate<0,44.997030,0> translate<46.522122,-1.535000,16.477878> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.875000,-1.535000,15.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.875000,-1.535000,16.125000>}
box{<0,0,-0.150000><0.437500,0.035000,0.150000> rotate<0,90.000000,0> translate<46.875000,-1.535000,16.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.931978,0.000000,29.631725>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.931978,0.000000,29.681978>}
box{<0,0,-0.075000><0.050253,0.035000,0.075000> rotate<0,90.000000,0> translate<46.931978,0.000000,29.681978> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.687500,0.000000,26.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.937500,0.000000,26.687500>}
box{<0,0,-0.075000><1.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<45.687500,0.000000,26.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.871319,0.000000,30.692388>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.000000,0.000000,31.821069>}
box{<0,0,-0.075000><1.596196,0.035000,0.075000> rotate<0,-44.997030,0> translate<45.871319,0.000000,30.692388> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.687500,0.000000,42.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.000000,0.000000,41.062500>}
box{<0,0,-0.075000><1.856155,0.035000,0.075000> rotate<0,44.997030,0> translate<45.687500,0.000000,42.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.000000,0.000000,31.821069>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.000000,0.000000,41.062500>}
box{<0,0,-0.075000><9.241431,0.035000,0.075000> rotate<0,90.000000,0> translate<47.000000,0.000000,41.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.875000,-1.535000,15.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.062500,-1.535000,15.500000>}
box{<0,0,-0.150000><0.265165,0.035000,0.150000> rotate<0,44.997030,0> translate<46.875000,-1.535000,15.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.062500,0.000000,15.359072>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.062500,0.000000,15.500000>}
box{<0,0,-0.150000><0.140928,0.035000,0.150000> rotate<0,90.000000,0> translate<47.062500,0.000000,15.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.250000,-1.535000,28.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.187500,-1.535000,26.062500>}
box{<0,0,-0.075000><2.740039,0.035000,0.075000> rotate<0,44.997030,0> translate<45.250000,-1.535000,28.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.632356,0.000000,14.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.285531,0.000000,13.721825>}
box{<0,0,-0.150000><0.923729,0.035000,0.150000> rotate<0,44.997030,0> translate<46.632356,0.000000,14.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.215672,0.000000,29.278172>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.285531,0.000000,29.278172>}
box{<0,0,-0.075000><0.069859,0.035000,0.075000> rotate<0,0.000000,0> translate<47.215672,0.000000,29.278172> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<45.250000,-1.535000,2.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.312500,-1.535000,2.875000>}
box{<0,0,-0.075000><2.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<45.250000,-1.535000,2.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<44.937500,-1.535000,3.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.500000,-1.535000,3.375000>}
box{<0,0,-0.075000><2.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<44.937500,-1.535000,3.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.224872,0.000000,30.349872>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.562500,0.000000,31.687500>}
box{<0,0,-0.075000><1.891692,0.035000,0.075000> rotate<0,-44.997030,0> translate<46.224872,0.000000,30.349872> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.687500,0.000000,43.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.562500,0.000000,42.312500>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,44.997030,0> translate<46.687500,0.000000,43.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.562500,0.000000,31.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.562500,0.000000,42.312500>}
box{<0,0,-0.075000><10.625000,0.035000,0.075000> rotate<0,90.000000,0> translate<47.562500,0.000000,42.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.102881,-1.535000,17.540378>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.580759,-1.535000,17.062500>}
box{<0,0,-0.150000><0.675822,0.035000,0.150000> rotate<0,44.997030,0> translate<47.102881,-1.535000,17.540378> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<46.931978,0.000000,13.368272>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<47.585119,0.000000,12.677234>}
box{<0,0,-0.140000><0.950855,0.035000,0.140000> rotate<0,46.611855,0> translate<46.931978,0.000000,13.368272> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<46.714463,0.000000,15.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.639084,0.000000,14.075378>}
box{<0,0,-0.150000><1.307613,0.035000,0.150000> rotate<0,44.997030,0> translate<46.714463,0.000000,15.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.612119,0.000000,28.924619>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.639084,0.000000,28.924619>}
box{<0,0,-0.075000><0.026966,0.035000,0.075000> rotate<0,0.000000,0> translate<47.612119,0.000000,28.924619> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.639084,0.000000,14.075378>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.674622,0.000000,14.075378>}
box{<0,0,-0.075000><0.035537,0.035000,0.075000> rotate<0,0.000000,0> translate<47.639084,0.000000,14.075378> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<38.125000,-1.535000,44.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.750000,-1.535000,44.000000>}
box{<0,0,-0.075000><9.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<38.125000,-1.535000,44.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.312500,-1.535000,2.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.812500,-1.535000,2.375000>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,44.997030,0> translate<47.312500,-1.535000,2.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.187500,-1.535000,26.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.812500,-1.535000,26.062500>}
box{<0,0,-0.075000><0.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<47.187500,-1.535000,26.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.812500,-1.535000,26.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.875000,-1.535000,26.125000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<47.812500,-1.535000,26.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.580759,-1.535000,17.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.937500,-1.535000,17.062500>}
box{<0,0,-0.150000><0.356741,0.035000,0.150000> rotate<0,0.000000,0> translate<47.580759,-1.535000,17.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.500000,0.000000,27.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.946066,0.000000,28.571066>}
box{<0,0,-0.075000><2.045046,0.035000,0.075000> rotate<0,-44.997030,0> translate<46.500000,0.000000,27.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.062500,0.000000,15.359072>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.992641,0.000000,14.428931>}
box{<0,0,-0.150000><1.315417,0.035000,0.150000> rotate<0,44.997030,0> translate<47.062500,0.000000,15.359072> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.992641,0.000000,14.428931>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.992641,0.000000,14.444859>}
box{<0,0,-0.150000><0.015928,0.035000,0.150000> rotate<0,90.000000,0> translate<47.992641,0.000000,14.444859> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.946066,0.000000,28.571066>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.992641,0.000000,28.571066>}
box{<0,0,-0.075000><0.046575,0.035000,0.075000> rotate<0,0.000000,0> translate<47.946066,0.000000,28.571066> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.500000,-1.535000,3.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,-1.535000,2.812500>}
box{<0,0,-0.075000><0.840852,0.035000,0.075000> rotate<0,41.984442,0> translate<47.500000,-1.535000,3.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.875000,0.000000,26.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,0.000000,26.375000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,-44.997030,0> translate<47.875000,0.000000,26.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,0.000000,26.582106>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,0.000000,26.375000>}
box{<0,0,-0.075000><0.207106,0.035000,0.075000> rotate<0,-90.000000,0> translate<48.125000,0.000000,26.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.578425,0.000000,30.015925>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,0.000000,31.562500>}
box{<0,0,-0.075000><2.187187,0.035000,0.075000> rotate<0,-44.997030,0> translate<46.578425,0.000000,30.015925> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.750000,0.000000,44.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,0.000000,43.625000>}
box{<0,0,-0.075000><0.530330,0.035000,0.075000> rotate<0,44.997030,0> translate<47.750000,0.000000,44.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,0.000000,31.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,0.000000,43.625000>}
box{<0,0,-0.075000><12.062500,0.035000,0.075000> rotate<0,90.000000,0> translate<48.125000,0.000000,43.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.937500,0.000000,17.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.187497,0.000000,17.062500>}
box{<0,0,-0.150000><0.249997,0.035000,0.150000> rotate<0,0.000000,0> translate<47.937500,0.000000,17.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.285531,0.000000,29.278172>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.194859,0.000000,30.187500>}
box{<0,0,-0.150000><1.285984,0.035000,0.150000> rotate<0,-44.997030,0> translate<47.285531,0.000000,29.278172> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.639084,0.000000,28.924619>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.276966,0.000000,29.562500>}
box{<0,0,-0.150000><0.902100,0.035000,0.150000> rotate<0,-44.997030,0> translate<47.639084,0.000000,28.924619> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.937500,0.000000,26.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.346194,0.000000,28.096194>}
box{<0,0,-0.075000><1.992194,0.035000,0.075000> rotate<0,-44.997030,0> translate<46.937500,0.000000,26.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.346194,0.000000,28.096194>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.346194,0.000000,28.217513>}
box{<0,0,-0.075000><0.121319,0.035000,0.075000> rotate<0,90.000000,0> translate<48.346194,0.000000,28.217513> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.937500,0.000000,16.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.355391,0.000000,16.187500>}
box{<0,0,-0.150000><0.417891,0.035000,0.150000> rotate<0,0.000000,0> translate<47.937500,0.000000,16.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.194859,0.000000,30.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.437500,0.000000,30.187500>}
box{<0,0,-0.150000><0.242641,0.035000,0.150000> rotate<0,0.000000,0> translate<48.194859,0.000000,30.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.437500,-1.535000,30.419238>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.437500,-1.535000,30.187500>}
box{<0,0,-0.150000><0.231738,0.035000,0.150000> rotate<0,-90.000000,0> translate<48.437500,-1.535000,30.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.931978,0.000000,29.681978>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.625000,0.000000,31.375000>}
box{<0,0,-0.075000><2.394294,0.035000,0.075000> rotate<0,-44.997030,0> translate<46.931978,0.000000,29.681978> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.625000,0.000000,31.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.625000,0.000000,41.812500>}
box{<0,0,-0.075000><10.437500,0.035000,0.075000> rotate<0,90.000000,0> translate<48.625000,0.000000,41.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<46.835788,0.000000,26.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.699747,0.000000,27.863959>}
box{<0,0,-0.075000><2.636037,0.035000,0.075000> rotate<0,-44.997030,0> translate<46.835788,0.000000,26.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.062500,-1.535000,44.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.812500,-1.535000,44.812500>}
box{<0,0,-0.075000><11.750000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.062500,-1.535000,44.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.875000,0.000000,17.082103>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.875000,0.000000,17.375000>}
box{<0,0,-0.150000><0.292897,0.035000,0.150000> rotate<0,90.000000,0> translate<48.875000,0.000000,17.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.875000,-1.535000,17.606741>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.875000,-1.535000,17.375000>}
box{<0,0,-0.150000><0.231741,0.035000,0.150000> rotate<0,-90.000000,0> translate<48.875000,-1.535000,17.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.022122,-1.535000,18.459619>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.875000,-1.535000,17.606741>}
box{<0,0,-0.150000><1.206152,0.035000,0.150000> rotate<0,44.997030,0> translate<48.022122,-1.535000,18.459619> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.437500,-1.535000,30.419238>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.040381,-1.535000,31.022119>}
box{<0,0,-0.150000><0.852603,0.035000,0.150000> rotate<0,-44.997030,0> translate<48.437500,-1.535000,30.419238> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.355391,0.000000,16.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.053300,0.000000,15.489591>}
box{<0,0,-0.150000><0.986993,0.035000,0.150000> rotate<0,44.997030,0> translate<48.355391,0.000000,16.187500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.053300,0.000000,15.489591>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.053300,0.000000,15.509200>}
box{<0,0,-0.075000><0.019609,0.035000,0.075000> rotate<0,90.000000,0> translate<49.053300,0.000000,15.509200> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,0.000000,26.582106>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.053300,0.000000,27.510406>}
box{<0,0,-0.075000><1.312814,0.035000,0.075000> rotate<0,-44.997030,0> translate<48.125000,0.000000,26.582106> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.276966,0.000000,29.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.125000,0.000000,29.562500>}
box{<0,0,-0.150000><0.848034,0.035000,0.150000> rotate<0,0.000000,0> translate<48.276966,0.000000,29.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.625000,-1.535000,36.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.187500,-1.535000,36.937500>}
box{<0,0,-0.075000><7.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<41.625000,-1.535000,36.937500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<47.210000,0.000000,8.535000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<49.260944,0.000000,8.915000>}
box{<0,0,-0.140000><2.085850,0.035000,0.140000> rotate<0,-10.496067,0> translate<47.210000,0.000000,8.535000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.625000,0.000000,41.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.375000,0.000000,42.562500>}
box{<0,0,-0.075000><1.060660,0.035000,0.075000> rotate<0,-44.997030,0> translate<48.625000,0.000000,41.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.812500,0.000000,44.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.375000,0.000000,44.250000>}
box{<0,0,-0.075000><0.795495,0.035000,0.075000> rotate<0,44.997030,0> translate<48.812500,0.000000,44.812500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.375000,0.000000,42.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.375000,0.000000,44.250000>}
box{<0,0,-0.075000><1.687500,0.035000,0.075000> rotate<0,90.000000,0> translate<49.375000,0.000000,44.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.187497,0.000000,17.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.406853,0.000000,15.843144>}
box{<0,0,-0.150000><1.724430,0.035000,0.150000> rotate<0,44.997030,0> translate<48.187497,0.000000,17.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.125000,-1.535000,29.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.419244,-1.535000,29.562500>}
box{<0,0,-0.150000><0.294244,0.035000,0.150000> rotate<0,0.000000,0> translate<49.125000,-1.535000,29.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<37.812500,-1.535000,35.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.437500,-1.535000,35.312500>}
box{<0,0,-0.075000><11.625000,0.035000,0.075000> rotate<0,0.000000,0> translate<37.812500,-1.535000,35.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.875000,0.000000,17.082103>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.760406,0.000000,16.196697>}
box{<0,0,-0.150000><1.252154,0.035000,0.150000> rotate<0,44.997030,0> translate<48.875000,0.000000,17.082103> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.740803,0.000000,16.196697>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.760406,0.000000,16.196697>}
box{<0,0,-0.075000><0.019603,0.035000,0.075000> rotate<0,0.000000,0> translate<49.740803,0.000000,16.196697> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<48.977881,-1.535000,19.040378>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.768259,-1.535000,18.250000>}
box{<0,0,-0.150000><1.117763,0.035000,0.150000> rotate<0,44.997030,0> translate<48.977881,-1.535000,19.040378> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.768259,-1.535000,18.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.812500,-1.535000,18.250000>}
box{<0,0,-0.150000><0.044241,0.035000,0.150000> rotate<0,0.000000,0> translate<49.768259,-1.535000,18.250000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<49.875000,0.000000,7.715000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<49.875000,0.000000,8.490944>}
box{<0,0,-0.140000><0.775944,0.035000,0.140000> rotate<0,90.000000,0> translate<49.875000,0.000000,8.490944> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.875000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.877000,0.000000,7.502000>}
box{<0,0,-0.150000><0.002828,0.035000,0.150000> rotate<0,-44.997030,0> translate<49.875000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<49.875000,0.000000,7.715000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<49.877000,0.000000,7.502000>}
box{<0,0,-0.140000><0.213009,0.035000,0.140000> rotate<0,89.456123,0> translate<49.875000,0.000000,7.715000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<47.139997,0.000000,9.035000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<49.895000,0.000000,9.485000>}
box{<0,0,-0.140000><2.791513,0.035000,0.140000> rotate<0,-9.276113,0> translate<47.139997,0.000000,9.035000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.419244,-1.535000,29.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.959622,-1.535000,30.102878>}
box{<0,0,-0.150000><0.764210,0.035000,0.150000> rotate<0,-44.997030,0> translate<49.419244,-1.535000,29.562500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.437500,0.000000,35.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.125000,0.000000,35.312500>}
box{<0,0,-0.075000><0.687500,0.035000,0.075000> rotate<0,0.000000,0> translate<49.437500,0.000000,35.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.375000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.375000,0.000000,5.875000>}
box{<0,0,-0.150000><1.625000,0.035000,0.150000> rotate<0,-90.000000,0> translate<50.375000,0.000000,5.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.897122,-1.535000,19.959619>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.419241,-1.535000,19.437500>}
box{<0,0,-0.150000><0.738387,0.035000,0.150000> rotate<0,44.997030,0> translate<49.897122,-1.535000,19.959619> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.125000,-1.535000,36.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.562500,-1.535000,36.125000>}
box{<0,0,-0.075000><9.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<41.125000,-1.535000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<41.875000,-1.535000,37.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.625000,-1.535000,37.750000>}
box{<0,0,-0.075000><8.750000,0.035000,0.075000> rotate<0,0.000000,0> translate<41.875000,-1.535000,37.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.040381,-1.535000,23.272119>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.705763,-1.535000,23.937500>}
box{<0,0,-0.150000><0.940991,0.035000,0.150000> rotate<0,-44.997030,0> translate<50.040381,-1.535000,23.272119> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.419241,-1.535000,19.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.750000,-1.535000,19.437500>}
box{<0,0,-0.150000><0.330759,0.035000,0.150000> rotate<0,0.000000,0> translate<50.419241,-1.535000,19.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.705763,-1.535000,23.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.750000,-1.535000,23.937500>}
box{<0,0,-0.150000><0.044237,0.035000,0.150000> rotate<0,0.000000,0> translate<50.705763,-1.535000,23.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.750000,0.000000,19.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.762244,0.000000,19.437500>}
box{<0,0,-0.150000><0.012244,0.035000,0.150000> rotate<0,0.000000,0> translate<50.750000,0.000000,19.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.812500,0.000000,18.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.805144,0.000000,17.257356>}
box{<0,0,-0.150000><1.403810,0.035000,0.150000> rotate<0,44.997030,0> translate<49.812500,0.000000,18.250000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.406853,0.000000,27.156853>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.812500,0.000000,28.562500>}
box{<0,0,-0.075000><1.987885,0.035000,0.075000> rotate<0,-44.997030,0> translate<49.406853,0.000000,27.156853> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.125000,0.000000,35.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.812500,0.000000,34.625000>}
box{<0,0,-0.075000><0.972272,0.035000,0.075000> rotate<0,44.997030,0> translate<50.125000,0.000000,35.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.812500,0.000000,28.562500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.812500,0.000000,34.625000>}
box{<0,0,-0.075000><6.062500,0.035000,0.075000> rotate<0,90.000000,0> translate<50.812500,0.000000,34.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.805144,0.000000,17.257356>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.821066,0.000000,17.257356>}
box{<0,0,-0.075000><0.015922,0.035000,0.075000> rotate<0,0.000000,0> translate<50.805144,0.000000,17.257356> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.625000,0.000000,37.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.875000,0.000000,37.750000>}
box{<0,0,-0.075000><0.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<50.625000,0.000000,37.750000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<50.875000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<50.880000,0.000000,7.505000>}
box{<0,0,-0.140000><0.007071,0.035000,0.140000> rotate<0,-44.997030,0> translate<50.875000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<50.880000,0.000000,7.505000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<50.880000,0.000000,8.590000>}
box{<0,0,-0.140000><1.085000,0.035000,0.140000> rotate<0,90.000000,0> translate<50.880000,0.000000,8.590000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<49.770000,0.000000,10.170000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<50.990000,0.000000,10.270000>}
box{<0,0,-0.140000><1.224091,0.035000,0.140000> rotate<0,-4.685591,0> translate<49.770000,0.000000,10.170000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.187500,0.000000,36.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.000000,0.000000,36.937500>}
box{<0,0,-0.075000><1.812500,0.035000,0.075000> rotate<0,0.000000,0> translate<49.187500,0.000000,36.937500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<49.730000,0.000000,10.650000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.086000,0.000000,10.743000>}
box{<0,0,-0.140000><1.359185,0.035000,0.140000> rotate<0,-3.923175,0> translate<49.730000,0.000000,10.650000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.071000,0.000000,12.051000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.101000,0.000000,12.252000>}
box{<0,0,-0.140000><0.203226,0.035000,0.140000> rotate<0,-81.505677,0> translate<51.071000,0.000000,12.051000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.750000,0.000000,23.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.137247,0.000000,23.937500>}
box{<0,0,-0.150000><0.387247,0.035000,0.150000> rotate<0,0.000000,0> translate<50.750000,0.000000,23.937500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.375000,0.000000,8.720000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.375000,0.000000,7.500000>}
box{<0,0,-0.140000><1.220000,0.035000,0.140000> rotate<0,-90.000000,0> translate<51.375000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<49.760406,0.000000,26.803300>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.375000,0.000000,28.417894>}
box{<0,0,-0.075000><2.283380,0.035000,0.075000> rotate<0,-44.997030,0> translate<49.760406,0.000000,26.803300> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.562500,0.000000,36.125000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.375000,0.000000,35.312500>}
box{<0,0,-0.075000><1.149049,0.035000,0.075000> rotate<0,44.997030,0> translate<50.562500,0.000000,36.125000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.375000,0.000000,28.417894>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.375000,0.000000,35.312500>}
box{<0,0,-0.075000><6.894606,0.035000,0.075000> rotate<0,90.000000,0> translate<51.375000,0.000000,35.312500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.108000,0.000000,12.295000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.395000,0.000000,13.037000>}
box{<0,0,-0.140000><0.795571,0.035000,0.140000> rotate<0,-68.849362,0> translate<51.108000,0.000000,12.295000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.528172,0.000000,25.035531>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.535531,0.000000,25.035531>}
box{<0,0,-0.075000><0.007359,0.035000,0.075000> rotate<0,0.000000,0> translate<51.528172,0.000000,25.035531> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.591000,0.000000,12.081000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.591000,0.000000,12.262000>}
box{<0,0,-0.140000><0.181000,0.035000,0.140000> rotate<0,90.000000,0> translate<51.591000,0.000000,12.262000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.959622,-1.535000,22.352878>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.625000,-1.535000,23.018256>}
box{<0,0,-0.150000><0.940987,0.035000,0.150000> rotate<0,-44.997030,0> translate<50.959622,-1.535000,22.352878> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.625000,-1.535000,23.018256>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.625000,-1.535000,23.062500>}
box{<0,0,-0.150000><0.044244,0.035000,0.150000> rotate<0,90.000000,0> translate<51.625000,-1.535000,23.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.625000,0.000000,23.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.625000,0.000000,23.062500>}
box{<0,0,-0.150000><0.625000,0.035000,0.150000> rotate<0,-90.000000,0> translate<51.625000,0.000000,23.062500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.375000,0.000000,8.720000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.626000,0.000000,8.971128>}
box{<0,0,-0.140000><0.355058,0.035000,0.140000> rotate<0,-45.011649,0> translate<51.375000,0.000000,8.720000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.626000,0.000000,8.971128>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.629000,0.000000,9.510128>}
box{<0,0,-0.140000><0.539008,0.035000,0.140000> rotate<0,-89.675184,0> translate<51.626000,0.000000,8.971128> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.629000,0.000000,9.510128>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.629000,0.000000,9.730000>}
box{<0,0,-0.140000><0.219872,0.035000,0.140000> rotate<0,90.000000,0> translate<51.629000,0.000000,9.730000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.625000,0.000000,23.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.750000,0.000000,22.937500>}
box{<0,0,-0.150000><0.176777,0.035000,0.150000> rotate<0,44.997030,0> translate<51.625000,0.000000,23.062500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<45.932500,-1.535000,48.125000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<51.812500,-1.535000,48.125000>}
box{<0,0,-0.406400><5.880000,0.035000,0.406400> rotate<0,0.000000,0> translate<45.932500,-1.535000,48.125000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.875000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.875000,0.000000,5.875000>}
box{<0,0,-0.150000><1.625000,0.035000,0.150000> rotate<0,-90.000000,0> translate<51.875000,0.000000,5.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<50.762244,0.000000,19.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.881725,0.000000,18.318019>}
box{<0,0,-0.150000><1.583186,0.035000,0.150000> rotate<0,44.997030,0> translate<50.762244,0.000000,19.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.869481,0.000000,18.318019>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.881725,0.000000,18.318019>}
box{<0,0,-0.075000><0.012244,0.035000,0.075000> rotate<0,0.000000,0> translate<51.869481,0.000000,18.318019> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.137247,0.000000,23.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.881725,0.000000,24.681978>}
box{<0,0,-0.150000><1.052851,0.035000,0.150000> rotate<0,-44.997030,0> translate<51.137247,0.000000,23.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.869478,0.000000,24.681978>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.881725,0.000000,24.681978>}
box{<0,0,-0.075000><0.012247,0.035000,0.075000> rotate<0,0.000000,0> translate<51.869478,0.000000,24.681978> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.588000,0.000000,12.265000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<51.895000,0.000000,12.996997>}
box{<0,0,-0.140000><0.793768,0.035000,0.140000> rotate<0,-67.242351,0> translate<51.588000,0.000000,12.265000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.113959,0.000000,26.449747>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.000000,0.000000,28.335788>}
box{<0,0,-0.075000><2.667264,0.035000,0.075000> rotate<0,-44.997030,0> translate<50.113959,0.000000,26.449747> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<51.000000,0.000000,36.937500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.000000,0.000000,35.937500>}
box{<0,0,-0.075000><1.414214,0.035000,0.075000> rotate<0,44.997030,0> translate<51.000000,0.000000,36.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.000000,0.000000,28.335788>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.000000,0.000000,35.937500>}
box{<0,0,-0.075000><7.601712,0.035000,0.075000> rotate<0,90.000000,0> translate<52.000000,0.000000,35.937500> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.076000,0.000000,8.970872>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.079000,0.000000,9.509872>}
box{<0,0,-0.140000><0.539008,0.035000,0.140000> rotate<0,-89.675184,0> translate<52.076000,0.000000,8.970872> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.079000,0.000000,9.509872>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.079000,0.000000,9.570000>}
box{<0,0,-0.140000><0.060128,0.035000,0.140000> rotate<0,90.000000,0> translate<52.079000,0.000000,9.570000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.625000,0.000000,23.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.235281,0.000000,24.297781>}
box{<0,0,-0.150000><0.863068,0.035000,0.150000> rotate<0,-44.997030,0> translate<51.625000,0.000000,23.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.235281,0.000000,24.328425>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.235281,0.000000,24.297781>}
box{<0,0,-0.075000><0.030644,0.035000,0.075000> rotate<0,-90.000000,0> translate<52.235281,0.000000,24.297781> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.750000,0.000000,22.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.250000,0.000000,22.937500>}
box{<0,0,-0.150000><0.500000,0.035000,0.150000> rotate<0,0.000000,0> translate<51.750000,0.000000,22.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.235281,0.000000,24.328425>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.265925,0.000000,24.328425>}
box{<0,0,-0.075000><0.030644,0.035000,0.075000> rotate<0,0.000000,0> translate<52.235281,0.000000,24.328425> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.375000,0.000000,8.672000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.375000,0.000000,7.500000>}
box{<0,0,-0.140000><1.172000,0.035000,0.140000> rotate<0,-90.000000,0> translate<52.375000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.076000,0.000000,8.970872>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.375000,0.000000,8.672000>}
box{<0,0,-0.140000><0.422759,0.035000,0.140000> rotate<0,44.984752,0> translate<52.076000,0.000000,8.970872> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<51.535531,0.000000,25.035531>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.437500,0.000000,25.937500>}
box{<0,0,-0.150000><1.275576,0.035000,0.150000> rotate<0,-44.997030,0> translate<51.535531,0.000000,25.035531> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.500000,0.000000,20.528172>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.500000,0.000000,21.000000>}
box{<0,0,-0.150000><0.471828,0.035000,0.150000> rotate<0,90.000000,0> translate<52.500000,0.000000,21.000000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,45.375000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,45.250000>}
box{<0,0,-0.406400><0.125000,0.035000,0.406400> rotate<0,-90.000000,0> translate<52.500000,-1.535000,45.250000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,45.375000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,46.187500>}
box{<0,0,-0.406400><0.812500,0.035000,0.406400> rotate<0,90.000000,0> translate<52.500000,-1.535000,46.187500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,46.187500>}
box{<0,0,-0.406400><0.812500,0.035000,0.406400> rotate<0,-90.000000,0> translate<52.500000,-1.535000,46.187500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,0.000000,45.250000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,0.000000,47.000000>}
box{<0,0,-0.406400><1.750000,0.035000,0.406400> rotate<0,90.000000,0> translate<52.500000,0.000000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<51.812500,-1.535000,48.125000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,47.437500>}
box{<0,0,-0.406400><0.972272,0.035000,0.406400> rotate<0,44.997030,0> translate<51.812500,-1.535000,48.125000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,-1.535000,47.437500>}
box{<0,0,-0.406400><0.437500,0.035000,0.406400> rotate<0,90.000000,0> translate<52.500000,-1.535000,47.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.588834,0.000000,19.025125>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.588834,0.000000,19.036166>}
box{<0,0,-0.075000><0.011041,0.035000,0.075000> rotate<0,90.000000,0> translate<52.588834,0.000000,19.036166> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.588834,0.000000,19.025125>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.599875,0.000000,19.025125>}
box{<0,0,-0.150000><0.011041,0.035000,0.150000> rotate<0,0.000000,0> translate<52.588834,0.000000,19.025125> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.500000,0.000000,21.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.610281,0.000000,21.875000>}
box{<0,0,-0.150000><0.110281,0.035000,0.150000> rotate<0,0.000000,0> translate<52.500000,0.000000,21.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.500000,-1.535000,21.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.650000,-1.535000,20.850000>}
box{<0,0,-0.150000><0.212132,0.035000,0.150000> rotate<0,44.997030,0> translate<52.500000,-1.535000,21.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.467513,0.000000,26.096194>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.687500,0.000000,28.316181>}
box{<0,0,-0.075000><3.139536,0.035000,0.075000> rotate<0,-44.997030,0> translate<50.467513,0.000000,26.096194> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<50.875000,0.000000,37.750000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.687500,0.000000,35.937500>}
box{<0,0,-0.075000><2.563262,0.035000,0.075000> rotate<0,44.997030,0> translate<50.875000,0.000000,37.750000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.687500,0.000000,28.316181>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.687500,0.000000,35.937500>}
box{<0,0,-0.075000><7.621319,0.035000,0.075000> rotate<0,90.000000,0> translate<52.687500,0.000000,35.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.500000,-1.535000,21.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.775000,-1.535000,22.150000>}
box{<0,0,-0.150000><0.388909,0.035000,0.150000> rotate<0,-44.997030,0> translate<52.500000,-1.535000,21.875000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.875000,0.000000,8.693000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.875000,0.000000,7.500000>}
box{<0,0,-0.140000><1.193000,0.035000,0.140000> rotate<0,-90.000000,0> translate<52.875000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.250000,0.000000,22.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.933819,0.000000,23.621319>}
box{<0,0,-0.150000><0.967066,0.035000,0.150000> rotate<0,-44.997030,0> translate<52.250000,0.000000,22.937500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.933819,0.000000,23.621319>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<52.942388,0.000000,23.621319>}
box{<0,0,-0.075000><0.008569,0.035000,0.075000> rotate<0,0.000000,0> translate<52.933819,0.000000,23.621319> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.235281,0.000000,24.328425>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.125000,0.000000,25.218144>}
box{<0,0,-0.150000><1.258252,0.035000,0.150000> rotate<0,-44.997030,0> translate<52.235281,0.000000,24.328425> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.125000,0.000000,25.218144>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.125000,0.000000,25.500000>}
box{<0,0,-0.150000><0.281856,0.035000,0.150000> rotate<0,90.000000,0> translate<53.125000,0.000000,25.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<52.875000,0.000000,8.693000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.153000,0.000000,8.981000>}
box{<0,0,-0.140000><0.400285,0.035000,0.140000> rotate<0,-46.009151,0> translate<52.875000,0.000000,8.693000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.250000,-1.535000,17.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.250000,-1.535000,17.268259>}
box{<0,0,-0.150000><0.231741,0.035000,0.150000> rotate<0,-90.000000,0> translate<53.250000,-1.535000,17.268259> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.153000,0.000000,8.981000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.257000,0.000000,10.215000>}
box{<0,0,-0.140000><1.238375,0.035000,0.140000> rotate<0,-85.176945,0> translate<53.153000,0.000000,8.981000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.500000,0.000000,20.528172>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.295941,0.000000,19.732231>}
box{<0,0,-0.150000><1.125630,0.035000,0.150000> rotate<0,44.997030,0> translate<52.500000,0.000000,20.528172> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.375000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.375000,0.000000,5.875000>}
box{<0,0,-0.150000><1.625000,0.035000,0.150000> rotate<0,-90.000000,0> translate<53.375000,0.000000,5.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.250000,0.000000,17.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.375000,0.000000,17.625000>}
box{<0,0,-0.150000><0.176777,0.035000,0.150000> rotate<0,-44.997030,0> translate<53.250000,0.000000,17.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.375000,0.000000,18.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.375000,0.000000,17.625000>}
box{<0,0,-0.150000><0.625000,0.035000,0.150000> rotate<0,-90.000000,0> translate<53.375000,0.000000,17.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.599875,0.000000,19.025125>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.375000,0.000000,18.250000>}
box{<0,0,-0.150000><1.096192,0.035000,0.150000> rotate<0,44.997030,0> translate<52.599875,0.000000,19.025125> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.650000,-1.535000,20.850000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.500000,-1.535000,20.850000>}
box{<0,0,-0.150000><0.850000,0.035000,0.150000> rotate<0,0.000000,0> translate<52.650000,-1.535000,20.850000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.775000,-1.535000,22.150000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.500000,-1.535000,22.150000>}
box{<0,0,-0.150000><0.725000,0.035000,0.150000> rotate<0,0.000000,0> translate<52.775000,-1.535000,22.150000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.607000,0.000000,9.244587>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.613000,0.000000,8.981000>}
box{<0,0,-0.140000><0.263656,0.035000,0.140000> rotate<0,88.690157,0> translate<53.607000,0.000000,9.244587> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<52.588834,0.000000,23.974872>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<53.613962,0.000000,25.000000>}
box{<0,0,-0.152400><1.449750,0.035000,0.152400> rotate<0,-44.997030,0> translate<52.588834,0.000000,23.974872> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<53.649494,0.000000,22.914213>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<53.649494,0.000000,22.899494>}
box{<0,0,-0.075000><0.014719,0.035000,0.075000> rotate<0,-90.000000,0> translate<53.649494,0.000000,22.899494> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.610281,0.000000,21.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.649494,0.000000,22.914213>}
box{<0,0,-0.150000><1.469668,0.035000,0.150000> rotate<0,-44.997030,0> translate<52.610281,0.000000,21.875000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<53.649494,0.000000,20.085784>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<53.664216,0.000000,20.085784>}
box{<0,0,-0.152400><0.014722,0.035000,0.152400> rotate<0,0.000000,0> translate<53.649494,0.000000,20.085784> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.607000,0.000000,9.244587>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.717000,0.000000,10.215000>}
box{<0,0,-0.140000><0.976627,0.035000,0.140000> rotate<0,-83.527394,0> translate<53.607000,0.000000,9.244587> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<52.500000,0.000000,45.250000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<53.775000,0.000000,43.975000>}
box{<0,0,-0.406400><1.803122,0.035000,0.406400> rotate<0,44.997030,0> translate<52.500000,0.000000,45.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.250000,-1.535000,17.268259>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.790381,-1.535000,16.727878>}
box{<0,0,-0.150000><0.764214,0.035000,0.150000> rotate<0,44.997030,0> translate<53.250000,-1.535000,17.268259> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<52.942388,0.000000,19.378678>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.821066,0.000000,18.500000>}
box{<0,0,-0.150000><1.242639,0.035000,0.150000> rotate<0,44.997030,0> translate<52.942388,0.000000,19.378678> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.875000,0.000000,8.719000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.875000,0.000000,7.500000>}
box{<0,0,-0.140000><1.219000,0.035000,0.140000> rotate<0,-90.000000,0> translate<53.875000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.613000,0.000000,8.981000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<53.875000,0.000000,8.719000>}
box{<0,0,-0.140000><0.370524,0.035000,0.140000> rotate<0,44.997030,0> translate<53.613000,0.000000,8.981000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.821066,0.000000,18.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.125000,0.000000,18.500000>}
box{<0,0,-0.150000><0.303934,0.035000,0.150000> rotate<0,0.000000,0> translate<53.821066,0.000000,18.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.125000,-1.535000,18.231741>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.125000,-1.535000,18.500000>}
box{<0,0,-0.150000><0.268259,0.035000,0.150000> rotate<0,90.000000,0> translate<54.125000,-1.535000,18.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<48.125000,-1.535000,2.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.250000,-1.535000,2.812500>}
box{<0,0,-0.075000><6.125000,0.035000,0.075000> rotate<0,0.000000,0> translate<48.125000,-1.535000,2.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.664216,0.000000,20.085784>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.312500,0.000000,19.437500>}
box{<0,0,-0.150000><0.916813,0.035000,0.150000> rotate<0,44.997030,0> translate<53.664216,0.000000,20.085784> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<54.375000,0.000000,8.650000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<54.375000,0.000000,7.500000>}
box{<0,0,-0.140000><1.150000,0.035000,0.140000> rotate<0,-90.000000,0> translate<54.375000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<53.613962,0.000000,25.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.426462,0.000000,25.812500>}
box{<0,0,-0.150000><1.149049,0.035000,0.150000> rotate<0,-44.997030,0> translate<53.613962,0.000000,25.000000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<54.375000,0.000000,8.650000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<54.655000,0.000000,8.930000>}
box{<0,0,-0.140000><0.395980,0.035000,0.140000> rotate<0,-44.997030,0> translate<54.375000,0.000000,8.650000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<54.645000,0.000000,9.140000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<54.655000,0.000000,8.930000>}
box{<0,0,-0.140000><0.210238,0.035000,0.140000> rotate<0,87.267929,0> translate<54.645000,0.000000,9.140000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.125000,-1.535000,18.231741>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.709622,-1.535000,17.647119>}
box{<0,0,-0.150000><0.826780,0.035000,0.150000> rotate<0,44.997030,0> translate<54.125000,-1.535000,18.231741> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.250000,-1.535000,2.812500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.812500,-1.535000,3.375000>}
box{<0,0,-0.075000><0.795495,0.035000,0.075000> rotate<0,-44.997030,0> translate<54.250000,-1.535000,2.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,0.000000,5.875000>}
box{<0,0,-0.150000><1.625000,0.035000,0.150000> rotate<0,-90.000000,0> translate<54.875000,0.000000,5.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<47.812500,-1.535000,2.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<55.062500,-1.535000,2.375000>}
box{<0,0,-0.075000><7.250000,0.035000,0.075000> rotate<0,0.000000,0> translate<47.812500,-1.535000,2.375000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<55.085000,0.000000,8.930000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<55.085000,0.000000,9.139997>}
box{<0,0,-0.140000><0.209997,0.035000,0.140000> rotate<0,90.000000,0> translate<55.085000,0.000000,9.139997> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<55.062500,-1.535000,2.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<55.312500,-1.535000,2.625000>}
box{<0,0,-0.075000><0.353553,0.035000,0.075000> rotate<0,-44.997030,0> translate<55.062500,-1.535000,2.375000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<55.085000,0.000000,8.930000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<55.375000,0.000000,8.640000>}
box{<0,0,-0.140000><0.410122,0.035000,0.140000> rotate<0,44.997030,0> translate<55.085000,0.000000,8.930000> }
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<55.375000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.140000 translate<55.375000,0.000000,8.640000>}
box{<0,0,-0.140000><1.140000,0.035000,0.140000> rotate<0,90.000000,0> translate<55.375000,0.000000,8.640000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.312500,0.000000,19.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<55.730000,0.000000,19.437500>}
box{<0,0,-0.150000><1.417500,0.035000,0.150000> rotate<0,0.000000,0> translate<54.312500,0.000000,19.437500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<55.312500,0.000000,2.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.125000,0.000000,3.437500>}
box{<0,0,-0.075000><1.149049,0.035000,0.075000> rotate<0,-44.997030,0> translate<55.312500,0.000000,2.625000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.125000,0.000000,4.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.125000,0.000000,3.437500>}
box{<0,0,-0.075000><1.437500,0.035000,0.075000> rotate<0,-90.000000,0> translate<56.125000,0.000000,3.437500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<53.775000,0.000000,43.975000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<56.187500,0.000000,43.975000>}
box{<0,0,-0.406400><2.412500,0.035000,0.406400> rotate<0,0.000000,0> translate<53.775000,0.000000,43.975000> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<56.187500,0.000000,42.000000>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<56.187500,0.000000,43.975000>}
box{<0,0,-0.508000><1.975000,0.035000,0.508000> rotate<0,90.000000,0> translate<56.187500,0.000000,43.975000> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<56.187500,0.000000,43.975000>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<56.225000,0.000000,44.000000>}
box{<0,0,-0.508000><0.045069,0.035000,0.508000> rotate<0,-33.687844,0> translate<56.187500,0.000000,43.975000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<55.730000,0.000000,19.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.333750,0.000000,20.041250>}
box{<0,0,-0.150000><0.853831,0.035000,0.150000> rotate<0,-44.997030,0> translate<55.730000,0.000000,19.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.333750,0.000000,20.041250>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.600000,0.000000,19.775000>}
box{<0,0,-0.150000><0.376534,0.035000,0.150000> rotate<0,44.997030,0> translate<56.333750,0.000000,20.041250> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.600000,0.000000,18.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.600000,0.000000,19.775000>}
box{<0,0,-0.150000><1.525000,0.035000,0.150000> rotate<0,90.000000,0> translate<56.600000,0.000000,19.775000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.600000,0.000000,23.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.600000,0.000000,25.682500>}
box{<0,0,-0.150000><1.995000,0.035000,0.150000> rotate<0,90.000000,0> translate<56.600000,0.000000,25.682500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<55.312500,-1.535000,2.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.625000,-1.535000,1.312500>}
box{<0,0,-0.075000><1.856155,0.035000,0.075000> rotate<0,44.997030,0> translate<55.312500,-1.535000,2.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.333750,0.000000,20.041250>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.730000,0.000000,20.437500>}
box{<0,0,-0.150000><0.560382,0.035000,0.150000> rotate<0,-44.997030,0> translate<56.333750,0.000000,20.041250> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.426462,0.000000,25.812500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.730000,0.000000,25.812500>}
box{<0,0,-0.150000><2.303538,0.035000,0.150000> rotate<0,0.000000,0> translate<54.426462,0.000000,25.812500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.600000,0.000000,25.682500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<56.730000,0.000000,25.812500>}
box{<0,0,-0.150000><0.183848,0.035000,0.150000> rotate<0,-44.997030,0> translate<56.600000,0.000000,25.682500> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<56.400000,0.000000,32.385000>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<56.765000,0.000000,32.750000>}
box{<0,0,-0.508000><0.516188,0.035000,0.508000> rotate<0,-44.997030,0> translate<56.400000,0.000000,32.385000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.812500,-1.535000,3.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.875000,-1.535000,3.375000>}
box{<0,0,-0.075000><2.062500,0.035000,0.075000> rotate<0,0.000000,0> translate<54.812500,-1.535000,3.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.875000,0.000000,4.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.875000,0.000000,3.375000>}
box{<0,0,-0.075000><1.312500,0.035000,0.075000> rotate<0,-90.000000,0> translate<56.875000,0.000000,3.375000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.125000,0.000000,4.875000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.875000,0.000000,5.625000>}
box{<0,0,-0.075000><1.060660,0.035000,0.075000> rotate<0,-44.997030,0> translate<56.125000,0.000000,4.875000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.875000,0.000000,5.625000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.875000,0.000000,7.500000>}
box{<0,0,-0.075000><1.875000,0.035000,0.075000> rotate<0,90.000000,0> translate<56.875000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<56.187500,0.000000,42.000000>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<57.337500,0.000000,40.912500>}
box{<0,0,-0.508000><1.582769,0.035000,0.508000> rotate<0,43.397111,0> translate<56.187500,0.000000,42.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.875000,0.000000,4.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<57.375000,0.000000,5.187500>}
box{<0,0,-0.075000><0.707107,0.035000,0.075000> rotate<0,-44.997030,0> translate<56.875000,0.000000,4.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<57.375000,0.000000,5.187500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<57.375000,0.000000,7.500000>}
box{<0,0,-0.075000><2.312500,0.035000,0.075000> rotate<0,90.000000,0> translate<57.375000,0.000000,7.500000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<57.337500,0.000000,40.912500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<57.662500,0.000000,40.912500>}
box{<0,0,-0.406400><0.325000,0.035000,0.406400> rotate<0,0.000000,0> translate<57.337500,0.000000,40.912500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<57.687500,-1.535000,40.937500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<57.687500,-1.535000,40.912500>}
box{<0,0,-0.406400><0.025000,0.035000,0.406400> rotate<0,-90.000000,0> translate<57.687500,-1.535000,40.912500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<57.662500,0.000000,40.912500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<57.687500,0.000000,40.937500>}
box{<0,0,-0.406400><0.035355,0.035000,0.406400> rotate<0,-44.997030,0> translate<57.662500,0.000000,40.912500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.875000,-1.535000,3.375000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<57.750000,-1.535000,2.500000>}
box{<0,0,-0.075000><1.237437,0.035000,0.075000> rotate<0,44.997030,0> translate<56.875000,-1.535000,3.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.875000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.875000,0.000000,6.062500>}
box{<0,0,-0.150000><1.437500,0.035000,0.150000> rotate<0,-90.000000,0> translate<57.875000,0.000000,6.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<56.625000,-1.535000,1.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<57.912500,-1.535000,1.312500>}
box{<0,0,-0.075000><1.287500,0.035000,0.075000> rotate<0,0.000000,0> translate<56.625000,-1.535000,1.312500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<57.750000,-1.535000,2.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<57.912500,-1.535000,2.500000>}
box{<0,0,-0.075000><0.162500,0.035000,0.075000> rotate<0,0.000000,0> translate<57.750000,-1.535000,2.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.875000,0.000000,6.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.125000,0.000000,5.812500>}
box{<0,0,-0.150000><0.353553,0.035000,0.150000> rotate<0,44.997030,0> translate<57.875000,0.000000,6.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.375000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.375000,0.000000,9.375000>}
box{<0,0,-0.150000><1.875000,0.035000,0.150000> rotate<0,90.000000,0> translate<58.375000,0.000000,9.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.375000,0.000000,9.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.562500,0.000000,9.562500>}
box{<0,0,-0.150000><0.265165,0.035000,0.150000> rotate<0,-44.997030,0> translate<58.375000,0.000000,9.375000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<57.687500,0.000000,40.937500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<58.562500,0.000000,40.937500>}
box{<0,0,-0.406400><0.875000,0.035000,0.406400> rotate<0,0.000000,0> translate<57.687500,0.000000,40.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.900000,0.000000,23.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.750000,0.000000,23.687500>}
box{<0,0,-0.150000><0.850000,0.035000,0.150000> rotate<0,0.000000,0> translate<57.900000,0.000000,23.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.500000,0.000000,10.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.787500,0.000000,10.275000>}
box{<0,0,-0.150000><0.406586,0.035000,0.150000> rotate<0,44.997030,0> translate<58.500000,0.000000,10.562500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<58.875000,0.000000,7.500000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<58.875000,0.000000,8.375000>}
box{<0,0,-0.152400><0.875000,0.035000,0.152400> rotate<0,90.000000,0> translate<58.875000,0.000000,8.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.875000,0.000000,8.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.875000,0.000000,8.437500>}
box{<0,0,-0.150000><0.062500,0.035000,0.150000> rotate<0,90.000000,0> translate<58.875000,0.000000,8.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.900000,0.000000,18.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.937500,0.000000,18.250000>}
box{<0,0,-0.150000><1.037500,0.035000,0.150000> rotate<0,0.000000,0> translate<57.900000,0.000000,18.250000> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<56.765000,0.000000,32.750000>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<59.000000,0.000000,32.750000>}
box{<0,0,-0.508000><2.235000,0.035000,0.508000> rotate<0,0.000000,0> translate<56.765000,0.000000,32.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.875000,0.000000,8.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<59.312500,0.000000,8.875000>}
box{<0,0,-0.150000><0.618718,0.035000,0.150000> rotate<0,-44.997030,0> translate<58.875000,0.000000,8.437500> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<59.625000,0.000000,44.000000>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<59.625000,0.000000,45.937500>}
box{<0,0,-0.508000><1.937500,0.035000,0.508000> rotate<0,90.000000,0> translate<59.625000,0.000000,45.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<59.312500,0.000000,8.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<59.900000,0.000000,8.875000>}
box{<0,0,-0.150000><0.587500,0.035000,0.150000> rotate<0,0.000000,0> translate<59.312500,0.000000,8.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<59.900000,0.000000,8.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<60.000000,0.000000,8.975000>}
box{<0,0,-0.150000><0.141421,0.035000,0.150000> rotate<0,-44.997030,0> translate<59.900000,0.000000,8.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.787500,0.000000,10.275000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<60.000000,0.000000,10.275000>}
box{<0,0,-0.150000><1.212500,0.035000,0.150000> rotate<0,0.000000,0> translate<58.787500,0.000000,10.275000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<59.212500,-1.535000,2.500000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<60.062500,-1.535000,2.500000>}
box{<0,0,-0.075000><0.850000,0.035000,0.075000> rotate<0,0.000000,0> translate<59.212500,-1.535000,2.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<59.212500,-1.535000,1.312500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<60.125000,-1.535000,1.312500>}
box{<0,0,-0.075000><0.912500,0.035000,0.075000> rotate<0,0.000000,0> translate<59.212500,-1.535000,1.312500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<59.437500,0.000000,40.937500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<60.312500,0.000000,40.937500>}
box{<0,0,-0.406400><0.875000,0.035000,0.406400> rotate<0,0.000000,0> translate<59.437500,0.000000,40.937500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<57.687500,-1.535000,40.912500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<60.337500,-1.535000,40.912500>}
box{<0,0,-0.406400><2.650000,0.035000,0.406400> rotate<0,0.000000,0> translate<57.687500,-1.535000,40.912500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<60.312500,-1.535000,40.937500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<60.337500,-1.535000,40.912500>}
box{<0,0,-0.406400><0.035355,0.035000,0.406400> rotate<0,44.997030,0> translate<60.312500,-1.535000,40.937500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<60.337500,-1.535000,40.912500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<60.375000,-1.535000,40.912500>}
box{<0,0,-0.406400><0.037500,0.035000,0.406400> rotate<0,0.000000,0> translate<60.337500,-1.535000,40.912500> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<57.337500,0.000000,40.912500>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<61.925000,0.000000,40.912500>}
box{<0,0,-0.508000><4.587500,0.035000,0.508000> rotate<0,0.000000,0> translate<57.337500,0.000000,40.912500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<61.937500,0.000000,20.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<62.037500,0.000000,20.437500>}
box{<0,0,-0.150000><0.100000,0.035000,0.150000> rotate<0,0.000000,0> translate<61.937500,0.000000,20.437500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<60.375000,-1.535000,40.912500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<62.225000,-1.535000,39.062500>}
box{<0,0,-0.406400><2.616295,0.035000,0.406400> rotate<0,44.997030,0> translate<60.375000,-1.535000,40.912500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<61.937500,0.000000,20.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<62.750000,0.000000,19.625000>}
box{<0,0,-0.150000><1.149049,0.035000,0.150000> rotate<0,44.997030,0> translate<61.937500,0.000000,20.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<62.750000,0.000000,19.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<62.787500,0.000000,19.625000>}
box{<0,0,-0.150000><0.037500,0.035000,0.150000> rotate<0,0.000000,0> translate<62.750000,0.000000,19.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<62.037500,0.000000,20.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<62.787500,0.000000,21.187500>}
box{<0,0,-0.150000><1.060660,0.035000,0.150000> rotate<0,-44.997030,0> translate<62.037500,0.000000,20.437500> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<59.000000,0.000000,32.750000>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<62.875000,0.000000,36.625000>}
box{<0,0,-0.508000><5.480078,0.035000,0.508000> rotate<0,-44.997030,0> translate<59.000000,0.000000,32.750000> }
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<62.875000,0.000000,38.712500>}
cylinder{<0,0,0><0,0.035000,0>0.508000 translate<62.875000,0.000000,36.625000>}
box{<0,0,-0.508000><2.087500,0.035000,0.508000> rotate<0,-90.000000,0> translate<62.875000,0.000000,36.625000> }
cylinder{<0,0,0><0,0.035000,0>0.304800 translate<63.825000,0.000000,40.875000>}
cylinder{<0,0,0><0,0.035000,0>0.304800 translate<63.825000,0.000000,40.912500>}
box{<0,0,-0.304800><0.037500,0.035000,0.304800> rotate<0,90.000000,0> translate<63.825000,0.000000,40.912500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<43.250000,-1.535000,4.375000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<64.687500,-1.535000,4.375000>}
box{<0,0,-0.406400><21.437500,0.035000,0.406400> rotate<0,0.000000,0> translate<43.250000,-1.535000,4.375000> }
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<64.087500,0.000000,19.625000>}
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<65.037500,0.000000,19.625000>}
box{<0,0,-0.203200><0.950000,0.035000,0.203200> rotate<0,0.000000,0> translate<64.087500,0.000000,19.625000> }
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<64.087500,0.000000,21.187500>}
cylinder{<0,0,0><0,0.035000,0>0.203200 translate<65.037500,0.000000,21.187500>}
box{<0,0,-0.203200><0.950000,0.035000,0.203200> rotate<0,0.000000,0> translate<64.087500,0.000000,21.187500> }
cylinder{<0,0,0><0,0.035000,0>0.304800 translate<63.825000,0.000000,40.875000>}
cylinder{<0,0,0><0,0.035000,0>0.304800 translate<65.350000,0.000000,40.875000>}
box{<0,0,-0.304800><1.525000,0.035000,0.304800> rotate<0,0.000000,0> translate<63.825000,0.000000,40.875000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<62.225000,-1.535000,39.062500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<67.187500,-1.535000,39.062500>}
box{<0,0,-0.406400><4.962500,0.035000,0.406400> rotate<0,0.000000,0> translate<62.225000,-1.535000,39.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<66.337500,0.000000,19.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<67.250000,0.000000,19.625000>}
box{<0,0,-0.150000><0.912500,0.035000,0.150000> rotate<0,0.000000,0> translate<66.337500,0.000000,19.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<67.250000,0.000000,19.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<67.312500,0.000000,19.562500>}
box{<0,0,-0.150000><0.088388,0.035000,0.150000> rotate<0,44.997030,0> translate<67.250000,0.000000,19.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<66.337500,0.000000,21.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<67.312500,0.000000,21.187500>}
box{<0,0,-0.150000><0.975000,0.035000,0.150000> rotate<0,0.000000,0> translate<66.337500,0.000000,21.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<66.650000,0.000000,40.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<67.500000,0.000000,40.875000>}
box{<0,0,-0.150000><0.850000,0.035000,0.150000> rotate<0,0.000000,0> translate<66.650000,0.000000,40.875000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<67.312500,-1.535000,21.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<67.812500,-1.535000,21.187500>}
box{<0,0,-0.150000><0.500000,0.035000,0.150000> rotate<0,0.000000,0> translate<67.312500,-1.535000,21.187500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<64.687500,-1.535000,4.375000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<68.375000,-1.535000,8.062500>}
box{<0,0,-0.406400><5.214913,0.035000,0.406400> rotate<0,-44.997030,0> translate<64.687500,-1.535000,4.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<67.812500,-1.535000,21.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<68.375000,-1.535000,21.750000>}
box{<0,0,-0.150000><0.795495,0.035000,0.150000> rotate<0,-44.997030,0> translate<67.812500,-1.535000,21.187500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<68.375000,-1.535000,8.062500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<68.375000,-1.535000,21.750000>}
box{<0,0,-0.406400><13.687500,0.035000,0.406400> rotate<0,90.000000,0> translate<68.375000,-1.535000,21.750000> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<67.187500,-1.535000,39.062500>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<68.375000,-1.535000,37.875000>}
box{<0,0,-0.406400><1.679379,0.035000,0.406400> rotate<0,44.997030,0> translate<67.187500,-1.535000,39.062500> }
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<68.375000,-1.535000,21.750000>}
cylinder{<0,0,0><0,0.035000,0>0.406400 translate<68.375000,-1.535000,37.875000>}
box{<0,0,-0.406400><16.125000,0.035000,0.406400> rotate<0,90.000000,0> translate<68.375000,-1.535000,37.875000> }
object{ARC(12.761147,0.280000,359.880642,398.813491,0.036000) translate<42.323878,0.000000,9.166581>}
object{ARC(7.917253,0.280000,38.813494,46.698230,0.036000) translate<46.098197,0.000000,12.202672>}
object{ARC(12.288687,0.280000,359.946008,398.878846,0.036000) translate<42.356319,0.000000,9.151581>}
object{ARC(59.241391,0.280000,44.570085,45.591929,0.036000) translate<9.719628,0.000000,-24.709603>}
object{ARC(1.503009,0.280000,0.190604,90.190604,0.036000) translate<52.214000,0.000000,10.210000>}
object{ARC(0.508600,0.280000,194.227502,284.227502,0.036000) translate<52.084000,0.000000,12.206000>}
object{ARC(0.003000,0.280000,180.000000,270.000000,0.036000) translate<51.591000,0.000000,12.265000>}
object{ARC(7.501184,0.280000,0.162656,21.660652,0.036000) translate<44.393847,0.000000,12.975703>}
object{ARC(3.005444,0.280000,23.635374,51.874918,0.036000) translate<48.612013,0.000000,14.539528>}
object{ARC(1.048000,0.280000,0.000000,90.000000,0.036000) translate<52.209000,0.000000,10.215000>}
object{ARC(0.978772,0.280000,190.299611,280.299611,0.036000) translate<52.034000,0.000000,12.226000>}
object{ARC(0.030806,0.280000,125.753887,215.753887,0.036000) translate<51.126000,0.000000,12.270000>}
object{ARC(4.039656,0.280000,31.515718,49.731759,0.036000) translate<47.502859,0.000000,13.467884>}
object{ARC(6.921391,0.280000,359.251621,380.749595,0.036000) translate<44.474200,0.000000,13.127403>}
object{ARC(1.086734,0.280000,355.249494,445.249494,0.036000) translate<50.996000,0.000000,9.660000>}
object{ARC(0.499009,0.280000,190.045232,282.574632,0.036000) translate<49.621359,0.000000,11.137041>}
object{ARC(0.742475,0.280000,125.224980,207.555556,0.036000) translate<49.788253,0.000000,11.393475>}
object{ARC(1.380337,0.280000,295.473396,408.529528,0.036000) translate<48.766328,0.000000,13.246147>}
object{ARC(3.726156,0.280000,218.839100,238.954396,0.036000) translate<51.602088,0.000000,17.472844>}
object{ARC(0.591575,0.280000,4.799836,94.799836,0.036000) translate<51.039500,0.000000,9.680500>}
object{ARC(1.016428,0.280000,193.642926,276.838861,0.036000) translate<49.648966,0.000000,11.179194>}
object{ARC(1.286578,0.280000,117.769471,200.100102,0.036000) translate<49.869438,0.000000,11.381597>}
object{ARC(0.782016,0.280000,297.566658,408.569970,0.036000) translate<48.908097,0.000000,13.213237>}
object{ARC(5.362019,0.280000,219.853782,235.501585,0.036000) translate<52.462522,0.000000,18.218631>}
object{ARC(1.227934,0.280000,232.155532,273.898689,0.036000) translate<47.056506,0.000000,10.260094>}
object{ARC(0.739059,0.280000,113.787516,210.300682,0.036000) translate<46.941241,0.000000,9.663303>}
object{ARC(2.028006,0.280000,293.851684,348.947595,0.036000) translate<45.823078,0.000000,12.194381>}
object{ARC(1.196859,0.280000,352.568137,396.792622,0.036000) translate<46.626663,0.000000,11.960409>}
object{ARC(0.941075,0.280000,2.740791,92.740791,0.036000) translate<49.940000,0.000000,8.545000>}
object{ARC(1.810425,0.280000,233.635975,275.379177,0.036000) translate<47.040278,0.000000,10.337453>}
object{ARC(1.238881,0.280000,121.775687,218.289221,0.036000) translate<46.939241,0.000000,9.647228>}
object{ARC(1.663953,0.280000,290.462196,345.558140,0.036000) translate<45.705153,0.000000,12.259384>}
object{ARC(0.618428,0.280000,347.987982,392.212659,0.036000) translate<46.711644,0.000000,11.973103>}
object{ARC(0.527678,0.280000,10.371736,100.371736,0.036000) translate<49.355944,0.000000,8.395944>}
object{ARC(4.041647,0.280000,35.796717,49.558209,0.036000) translate<43.956709,0.000000,9.938756>}
object{ARC(2.018322,0.280000,185.358916,275.358916,0.036000) translate<15.624500,-1.535000,45.634500>}
object{ARC(2.347000,0.280000,0.000000,90.000000,0.036000) translate<11.268000,-1.535000,45.446000>}
object{ARC(8.904944,0.280000,92.011092,117.302757,0.036000) translate<10.141500,-1.535000,38.893541>}
object{ARC(1.337134,0.280000,236.857674,297.302723,0.036000) translate<5.443541,-1.535000,47.994603>}
object{ARC(2.516569,0.280000,184.295681,274.295681,0.036000) translate<15.674500,-1.535000,45.634500>}
object{ARC(1.897000,0.280000,0.000000,90.000000,0.036000) translate<11.268000,-1.535000,45.446000>}
object{ARC(8.455234,0.280000,92.118140,117.409781,0.036000) translate<10.141503,-1.535000,38.893544>}
object{ARC(5.453603,0.280000,280.287507,297.409754,0.036000) translate<3.738553,-1.535000,51.240931>}
object{ARC(0.795000,0.280000,180.000000,270.000000,0.036000) translate<13.510000,-1.535000,43.420000>}
object{ARC(1.722588,0.280000,359.417915,449.417915,0.036000) translate<10.992500,-1.535000,43.437500>}
object{ARC(0.962659,0.280000,91.041627,181.041627,0.036000) translate<9.787500,-1.535000,44.197500>}
object{ARC(0.395000,0.280000,270.000000,360.000000,0.036000) translate<8.430000,-1.535000,43.940000>}
object{ARC(1.190009,0.280000,226.277126,270.000002,0.036000) translate<5.535000,-1.535000,44.735009>}
object{ARC(1.295000,0.280000,180.000000,270.000000,0.036000) translate<13.560000,-1.535000,43.420000>}
object{ARC(1.272622,0.280000,359.212092,449.212092,0.036000) translate<10.992500,-1.535000,43.437500>}
object{ARC(0.495000,0.280000,90.000000,180.000000,0.036000) translate<9.770000,-1.535000,44.215000>}
object{ARC(0.845000,0.280000,270.000000,360.000000,0.036000) translate<8.430000,-1.535000,43.940000>}
object{ARC(1.647516,0.280000,90.000012,119.949519,0.036000) translate<5.535000,-1.535000,41.447484>}
object{ARC(1.726597,0.280000,83.648815,173.648815,0.036000) translate<12.734000,-1.535000,38.909000>}
object{ARC(0.853913,0.280000,259.098000,349.098000,0.036000) translate<10.175500,-1.535000,39.144500>}
object{ARC(0.700000,0.280000,180.000000,270.000000,0.036000) translate<9.593000,-1.535000,39.006000>}
object{ARC(1.442425,0.280000,355.028539,445.028539,0.036000) translate<7.456000,-1.535000,39.131000>}
object{ARC(1.326059,0.280000,230.218218,270.000018,0.036000) translate<5.561000,-1.535000,41.894059>}
object{ARC(1.230909,0.280000,81.073351,171.073351,0.036000) translate<12.684000,-1.535000,38.909000>}
object{ARC(1.298581,0.280000,262.855829,352.855829,0.036000) translate<10.175500,-1.535000,39.144500>}
object{ARC(1.150000,0.280000,180.000000,270.000000,0.036000) translate<9.593000,-1.535000,39.006000>}
object{ARC(0.994884,0.280000,352.782122,442.782122,0.036000) translate<7.456000,-1.535000,39.131000>}
object{ARC(1.602884,0.280000,90.000011,121.962117,0.036000) translate<5.561000,-1.535000,38.515116>}
object{ARC(2.125000,0.280000,90.000000,180.000000,0.036000) translate<15.962500,-1.535000,36.500000>}
object{ARC(3.021609,0.280000,260.253999,350.253999,0.036000) translate<10.859500,-1.535000,37.011500>}
object{ARC(2.307012,0.280000,237.985101,263.827914,0.036000) translate<5.935538,-1.535000,36.831138>}
object{ARC(1.625000,0.280000,90.000000,180.000000,0.036000) translate<15.912500,-1.535000,36.500000>}
object{ARC(3.465950,0.280000,261.513374,351.513374,0.036000) translate<10.859500,-1.535000,37.011500>}
object{ARC(1.575116,0.280000,83.827764,120.762768,0.036000) translate<5.518147,-1.535000,32.521516>}
object{ARC(2.601522,0.280000,88.039487,178.039487,0.036000) translate<14.863000,-1.535000,37.025000>}
object{ARC(0.880778,0.280000,267.592391,357.592391,0.036000) translate<11.383000,-1.535000,37.151000>}
object{ARC(0.446163,0.280000,181.541219,271.541219,0.036000) translate<8.347000,-1.535000,36.714000>}
object{ARC(0.973844,0.280000,5.716447,95.716447,0.036000) translate<6.932000,-1.535000,36.605000>}
object{ARC(1.409253,0.280000,231.851433,270.000018,0.036000) translate<5.583000,-1.535000,38.983253>}
object{ARC(2.101884,0.280000,87.573203,177.573203,0.036000) translate<14.813000,-1.535000,37.025000>}
object{ARC(1.330516,0.280000,268.406468,358.406468,0.036000) translate<11.383000,-1.535000,37.151000>}
object{ARC(0.890181,0.280000,181.158633,271.158633,0.036000) translate<8.341000,-1.535000,36.708000>}
object{ARC(0.532828,0.280000,9.833564,99.833564,0.036000) translate<6.926000,-1.535000,36.599000>}
object{ARC(1.646128,0.280000,90.000010,121.925551,0.036000) translate<5.583000,-1.535000,35.477872>}
//Text
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<8.514700,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<8.514700,0.000000,59.014581>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,90.000000,0> translate<8.514700,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<8.514700,0.000000,58.264641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<9.514619,0.000000,58.264641>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<8.514700,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<9.514619,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<9.514619,0.000000,57.514700>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<9.514619,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.151856,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.151856,0.000000,57.514700>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<10.151856,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.151856,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,57.514700>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<10.151856,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,57.764678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<10.901797,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,57.764678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,58.764600>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,90.000000,0> translate<11.151775,0.000000,58.764600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,59.014581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<10.901797,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.151856,0.000000,59.014581>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<10.151856,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.789013,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.789013,0.000000,59.014581>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,90.000000,0> translate<11.789013,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.789013,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.288972,0.000000,58.514619>}
box{<0,0,-0.139700><0.707052,0.035000,0.139700> rotate<0,44.997209,0> translate<11.789013,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.288972,0.000000,58.514619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.788931,0.000000,59.014581>}
box{<0,0,-0.139700><0.707052,0.035000,0.139700> rotate<0,-44.997209,0> translate<12.288972,0.000000,58.514619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.788931,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.788931,0.000000,57.514700>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<12.788931,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.426169,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.926128,0.000000,57.514700>}
box{<0,0,-0.139700><0.499959,0.035000,0.139700> rotate<0,0.000000,0> translate<13.426169,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.676147,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.676147,0.000000,59.014581>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,90.000000,0> translate<13.676147,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.426169,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.926128,0.000000,59.014581>}
box{<0,0,-0.139700><0.499959,0.035000,0.139700> rotate<0,0.000000,0> translate<13.426169,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.517606,0.000000,57.264722>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.517525,0.000000,57.264722>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<14.517606,0.000000,57.264722> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.154763,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.154763,0.000000,59.014581>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,90.000000,0> translate<16.154763,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.154763,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.904703,0.000000,59.014581>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<16.154763,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.904703,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.154681,0.000000,58.764600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<16.904703,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.154681,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.154681,0.000000,58.264641>}
box{<0,0,-0.139700><0.499959,0.035000,0.139700> rotate<0,-90.000000,0> translate<17.154681,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.154681,0.000000,58.264641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.904703,0.000000,58.014659>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<16.904703,0.000000,58.014659> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.904703,0.000000,58.014659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.154763,0.000000,58.014659>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<16.154763,0.000000,58.014659> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.654722,0.000000,58.014659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.154681,0.000000,57.514700>}
box{<0,0,-0.139700><0.707049,0.035000,0.139700> rotate<0,44.997030,0> translate<16.654722,0.000000,58.014659> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.791837,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.541859,0.000000,59.014581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<18.541859,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.541859,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.041897,0.000000,59.014581>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<18.041897,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.041897,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.791919,0.000000,58.764600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<17.791919,0.000000,58.764600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.791919,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.791919,0.000000,57.764678>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,-90.000000,0> translate<17.791919,0.000000,57.764678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.791919,0.000000,57.764678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.041897,0.000000,57.514700>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<17.791919,0.000000,57.764678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.041897,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.541859,0.000000,57.514700>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<18.041897,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.541859,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.791837,0.000000,57.764678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<18.541859,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.791837,0.000000,57.764678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.791837,0.000000,58.264641>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,90.000000,0> translate<18.791837,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.791837,0.000000,58.264641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.291878,0.000000,58.264641>}
box{<0,0,-0.139700><0.499959,0.035000,0.139700> rotate<0,0.000000,0> translate<18.291878,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.429075,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.429075,0.000000,59.014581>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,90.000000,0> translate<19.429075,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.429075,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.179016,0.000000,59.014581>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<19.429075,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.179016,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.428994,0.000000,58.764600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<20.179016,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.428994,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.428994,0.000000,58.514619>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<20.428994,0.000000,58.514619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.428994,0.000000,58.514619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.179016,0.000000,58.264641>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<20.179016,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.179016,0.000000,58.264641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.428994,0.000000,58.014659>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<20.179016,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.428994,0.000000,58.014659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.428994,0.000000,57.764678>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<20.428994,0.000000,57.764678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.428994,0.000000,57.764678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.179016,0.000000,57.514700>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<20.179016,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.179016,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.429075,0.000000,57.514700>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<19.429075,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.429075,0.000000,58.264641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.179016,0.000000,58.264641>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<19.429075,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<21.066231,0.000000,57.264722>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<22.066150,0.000000,57.264722>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<21.066231,0.000000,57.264722> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<22.703388,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<22.703388,0.000000,57.514700>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<22.703388,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<22.703388,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<23.703306,0.000000,57.514700>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<22.703388,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<24.340544,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<24.340544,0.000000,58.014659>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,-90.000000,0> translate<24.340544,0.000000,58.014659> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<24.340544,0.000000,58.014659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<24.840503,0.000000,57.514700>}
box{<0,0,-0.139700><0.707049,0.035000,0.139700> rotate<0,44.997030,0> translate<24.340544,0.000000,58.014659> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<24.840503,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<25.340463,0.000000,58.014659>}
box{<0,0,-0.139700><0.707049,0.035000,0.139700> rotate<0,-44.997030,0> translate<24.840503,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<25.340463,0.000000,58.014659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<25.340463,0.000000,59.014581>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,90.000000,0> translate<25.340463,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<25.977700,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<25.977700,0.000000,57.514700>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<25.977700,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<25.977700,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<26.727641,0.000000,57.514700>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<25.977700,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<26.727641,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<26.977619,0.000000,57.764678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<26.727641,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<26.977619,0.000000,57.764678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<26.977619,0.000000,58.764600>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,90.000000,0> translate<26.977619,0.000000,58.764600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<26.977619,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<26.727641,0.000000,59.014581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<26.727641,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<26.727641,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<25.977700,0.000000,59.014581>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<25.977700,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.614775,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.364797,0.000000,59.014581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<28.364797,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.364797,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.864834,0.000000,59.014581>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<27.864834,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.864834,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.614856,0.000000,58.764600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<27.614856,0.000000,58.764600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.614856,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.614856,0.000000,58.514619>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<27.614856,0.000000,58.514619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.614856,0.000000,58.514619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.864834,0.000000,58.264641>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<27.614856,0.000000,58.514619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.864834,0.000000,58.264641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.364797,0.000000,58.264641>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<27.864834,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.364797,0.000000,58.264641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.614775,0.000000,58.014659>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<28.364797,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.614775,0.000000,58.014659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.614775,0.000000,57.764678>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<28.614775,0.000000,57.764678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.614775,0.000000,57.764678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.364797,0.000000,57.514700>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<28.364797,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<28.364797,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.864834,0.000000,57.514700>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<27.864834,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.864834,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<27.614856,0.000000,57.764678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<27.614856,0.000000,57.764678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<30.889169,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<30.889169,0.000000,58.014659>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,-90.000000,0> translate<30.889169,0.000000,58.014659> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<30.889169,0.000000,58.014659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<31.389128,0.000000,57.514700>}
box{<0,0,-0.139700><0.707049,0.035000,0.139700> rotate<0,44.997030,0> translate<30.889169,0.000000,58.014659> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<31.389128,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<31.889088,0.000000,58.014659>}
box{<0,0,-0.139700><0.707049,0.035000,0.139700> rotate<0,-44.997030,0> translate<31.389128,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<31.889088,0.000000,58.014659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<31.889088,0.000000,59.014581>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,90.000000,0> translate<31.889088,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.526325,0.000000,57.764678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.776303,0.000000,57.514700>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<32.526325,0.000000,57.764678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.776303,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.276266,0.000000,57.514700>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<32.776303,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.276266,0.000000,57.514700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.526244,0.000000,57.764678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<33.276266,0.000000,57.514700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.526244,0.000000,57.764678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.526244,0.000000,58.764600>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,90.000000,0> translate<33.526244,0.000000,58.764600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.526244,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.276266,0.000000,59.014581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<33.276266,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.276266,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.776303,0.000000,59.014581>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<32.776303,0.000000,59.014581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.776303,0.000000,59.014581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.526325,0.000000,58.764600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<32.526325,0.000000,58.764600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.526325,0.000000,58.764600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.526325,0.000000,58.514619>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<32.526325,0.000000,58.514619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.526325,0.000000,58.514619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.776303,0.000000,58.264641>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<32.526325,0.000000,58.514619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<32.776303,0.000000,58.264641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<33.526244,0.000000,58.264641>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<32.776303,0.000000,58.264641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,57.619200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,58.619119>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,90.000000,0> translate<36.264700,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.764659,0.000000,59.119081>}
box{<0,0,-0.139700><0.707052,0.035000,0.139700> rotate<0,-44.997209,0> translate<36.264700,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.764659,0.000000,59.119081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,58.619119>}
box{<0,0,-0.139700><0.707052,0.035000,0.139700> rotate<0,44.997209,0> translate<36.764659,0.000000,59.119081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,57.619200>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,-90.000000,0> translate<37.264619,0.000000,57.619200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,58.369141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,58.369141>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<36.264700,0.000000,58.369141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.901856,0.000000,57.619200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.901856,0.000000,58.619119>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,90.000000,0> translate<37.901856,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.901856,0.000000,58.119159>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.401816,0.000000,58.619119>}
box{<0,0,-0.139700><0.707049,0.035000,0.139700> rotate<0,-44.997030,0> translate<37.901856,0.000000,58.119159> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.401816,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.651797,0.000000,58.619119>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,0.000000,0> translate<38.401816,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.266153,0.000000,57.619200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.266153,0.000000,58.619119>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,90.000000,0> translate<39.266153,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.266153,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.516131,0.000000,58.619119>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,0.000000,0> translate<39.266153,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.516131,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.766112,0.000000,58.369141>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.996672,0> translate<39.516131,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.766112,0.000000,58.369141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.766112,0.000000,57.619200>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,-90.000000,0> translate<39.766112,0.000000,57.619200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.766112,0.000000,58.369141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.016094,0.000000,58.619119>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.996672,0> translate<39.766112,0.000000,58.369141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.016094,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.266072,0.000000,58.369141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<40.016094,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.266072,0.000000,58.369141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.266072,0.000000,57.619200>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,-90.000000,0> translate<40.266072,0.000000,57.619200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.903309,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.153288,0.000000,58.619119>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,0.000000,0> translate<40.903309,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.153288,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.153288,0.000000,57.619200>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,-90.000000,0> translate<41.153288,0.000000,57.619200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.903309,0.000000,57.619200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.403269,0.000000,57.619200>}
box{<0,0,-0.139700><0.499959,0.035000,0.139700> rotate<0,0.000000,0> translate<40.903309,0.000000,57.619200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.153288,0.000000,59.369059>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.153288,0.000000,59.119081>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,-90.000000,0> translate<41.153288,0.000000,59.119081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.994747,0.000000,57.619200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.994747,0.000000,58.619119>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,90.000000,0> translate<41.994747,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.994747,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.744688,0.000000,58.619119>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<41.994747,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.744688,0.000000,58.619119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.994666,0.000000,58.369141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<42.744688,0.000000,58.619119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.994666,0.000000,58.369141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.994666,0.000000,57.619200>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,-90.000000,0> translate<42.994666,0.000000,57.619200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,56.202100>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.014641,0.000000,56.452081>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<37.014641,0.000000,56.452081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.014641,0.000000,56.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.514678,0.000000,56.452081>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<36.514678,0.000000,56.452081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.514678,0.000000,56.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,56.202100>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<36.264700,0.000000,56.202100> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,56.202100>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,55.952119>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<36.264700,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.514678,0.000000,55.702141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<36.264700,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.514678,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.014641,0.000000,55.702141>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<36.514678,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.014641,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,55.452159>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<37.014641,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,55.452159>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,55.202178>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<37.264619,0.000000,55.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.264619,0.000000,55.202178>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.014641,0.000000,54.952200>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<37.014641,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.014641,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.514678,0.000000,54.952200>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<36.514678,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.514678,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<36.264700,0.000000,55.202178>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<36.264700,0.000000,55.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.901775,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.151834,0.000000,55.952119>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<38.151834,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.151834,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.901856,0.000000,55.702141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<37.901856,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.901856,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.901856,0.000000,55.202178>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,-90.000000,0> translate<37.901856,0.000000,55.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<37.901856,0.000000,55.202178>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.151834,0.000000,54.952200>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<37.901856,0.000000,55.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.151834,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<38.901775,0.000000,54.952200>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<38.151834,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.539013,0.000000,56.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.539013,0.000000,54.952200>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<39.539013,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.539013,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.788991,0.000000,55.952119>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<39.539013,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<39.788991,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.288953,0.000000,55.952119>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<39.788991,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.288953,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.538931,0.000000,55.702141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<40.288953,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.538931,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<40.538931,0.000000,54.952200>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,-90.000000,0> translate<40.538931,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.176169,0.000000,56.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.426147,0.000000,56.452081>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,0.000000,0> translate<41.176169,0.000000,56.452081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.426147,0.000000,56.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.426147,0.000000,54.952200>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<41.426147,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.176169,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<41.676128,0.000000,54.952200>}
box{<0,0,-0.139700><0.499959,0.035000,0.139700> rotate<0,0.000000,0> translate<41.176169,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.017547,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.517584,0.000000,54.952200>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<42.517584,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.517584,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.267606,0.000000,55.202178>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<42.267606,0.000000,55.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.267606,0.000000,55.202178>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.267606,0.000000,55.702141>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,90.000000,0> translate<42.267606,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.267606,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.517584,0.000000,55.952119>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<42.267606,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.517584,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.017547,0.000000,55.952119>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<42.517584,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.017547,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.267525,0.000000,55.702141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<43.017547,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.267525,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.267525,0.000000,55.452159>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<43.267525,0.000000,55.452159> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.267525,0.000000,55.452159>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<42.267606,0.000000,55.452159>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<42.267606,0.000000,55.452159> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.404722,0.000000,54.452241>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.654703,0.000000,54.452241>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,0.000000,0> translate<44.404722,0.000000,54.452241> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.654703,0.000000,54.452241>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.904681,0.000000,54.702222>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<44.654703,0.000000,54.452241> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.904681,0.000000,54.702222>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.904681,0.000000,55.952119>}
box{<0,0,-0.139700><1.249897,0.035000,0.139700> rotate<0,90.000000,0> translate<44.904681,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.904681,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.154741,0.000000,55.952119>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<44.154741,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.154741,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.904763,0.000000,55.702141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<43.904763,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.904763,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.904763,0.000000,55.202178>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,-90.000000,0> translate<43.904763,0.000000,55.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<43.904763,0.000000,55.202178>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.154741,0.000000,54.952200>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<43.904763,0.000000,55.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.154741,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<44.904681,0.000000,54.952200>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,0.000000,0> translate<44.154741,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<46.291859,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.791897,0.000000,54.952200>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<45.791897,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.791897,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.541919,0.000000,55.202178>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<45.541919,0.000000,55.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.541919,0.000000,55.202178>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.541919,0.000000,55.702141>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,90.000000,0> translate<45.541919,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.541919,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.791897,0.000000,55.952119>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<45.541919,0.000000,55.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.791897,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<46.291859,0.000000,55.952119>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<45.791897,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<46.291859,0.000000,55.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<46.541837,0.000000,55.702141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<46.291859,0.000000,55.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<46.541837,0.000000,55.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<46.541837,0.000000,55.452159>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<46.541837,0.000000,55.452159> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<46.541837,0.000000,55.452159>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<45.541919,0.000000,55.452159>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<45.541919,0.000000,55.452159> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<47.179075,0.000000,56.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<47.429053,0.000000,56.452081>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,0.000000,0> translate<47.179075,0.000000,56.452081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<47.429053,0.000000,56.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<47.429053,0.000000,54.952200>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<47.429053,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<47.179075,0.000000,54.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<47.679034,0.000000,54.952200>}
box{<0,0,-0.139700><0.499959,0.035000,0.139700> rotate<0,0.000000,0> translate<47.179075,0.000000,54.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.952200,0.000000,16.139619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.452159,0.000000,16.639581>}
box{<0,0,-0.139700><0.707052,0.035000,0.139700> rotate<0,-44.997209,0> translate<64.952200,0.000000,16.139619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.452159,0.000000,16.639581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.452159,0.000000,15.139700>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<65.452159,0.000000,15.139700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.952200,0.000000,15.139700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.952119,0.000000,15.139700>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<64.952200,0.000000,15.139700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.952119,-0.500000,12.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.952200,-0.500000,12.952200>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<64.952200,-0.500000,12.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.952200,-0.500000,12.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.952119,-0.500000,13.952119>}
box{<0,0,-0.139700><1.414099,0.035000,0.139700> rotate<0,-44.997030,0> translate<64.952200,-0.500000,12.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.952119,-0.500000,13.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.952119,-0.500000,14.202100>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,90.000000,0> translate<65.952119,-0.500000,14.202100> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.952119,-0.500000,14.202100>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.702141,-0.500000,14.452081>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<65.702141,-0.500000,14.452081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.702141,-0.500000,14.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.202178,-0.500000,14.452081>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<65.202178,-0.500000,14.452081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.202178,-0.500000,14.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.952200,-0.500000,14.202100>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<64.952200,-0.500000,14.202100> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.827200,-1.000000,12.202100>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.077178,-1.000000,12.452081>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<64.827200,-1.000000,12.202100> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.077178,-1.000000,12.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.577141,-1.000000,12.452081>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<65.077178,-1.000000,12.452081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.577141,-1.000000,12.452081>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.827119,-1.000000,12.202100>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<65.577141,-1.000000,12.452081> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.827119,-1.000000,12.202100>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.827119,-1.000000,11.952119>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<65.827119,-1.000000,11.952119> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.827119,-1.000000,11.952119>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.577141,-1.000000,11.702141>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<65.577141,-1.000000,11.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.577141,-1.000000,11.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.327159,-1.000000,11.702141>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,0.000000,0> translate<65.327159,-1.000000,11.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.577141,-1.000000,11.702141>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.827119,-1.000000,11.452159>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<65.577141,-1.000000,11.702141> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.827119,-1.000000,11.452159>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.827119,-1.000000,11.202178>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<65.827119,-1.000000,11.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.827119,-1.000000,11.202178>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.577141,-1.000000,10.952200>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<65.577141,-1.000000,10.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.577141,-1.000000,10.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.077178,-1.000000,10.952200>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<65.077178,-1.000000,10.952200> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.077178,-1.000000,10.952200>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.827200,-1.000000,11.202178>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<64.827200,-1.000000,11.202178> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.985359,-1.535000,8.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.985359,-1.535000,10.389581>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,90.000000,0> translate<64.985359,-1.535000,10.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.985359,-1.535000,10.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.735300,-1.535000,9.639641>}
box{<0,0,-0.139700><1.060576,0.035000,0.139700> rotate<0,44.997030,0> translate<64.985359,-1.535000,10.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<65.735300,-1.535000,9.639641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<64.735381,-1.535000,9.639641>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<64.735381,-1.535000,9.639641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<8.514700,0.000000,55.889619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<9.014659,0.000000,56.389581>}
box{<0,0,-0.139700><0.707052,0.035000,0.139700> rotate<0,-44.997209,0> translate<8.514700,0.000000,55.889619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<9.014659,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<9.014659,0.000000,54.889700>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<9.014659,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<8.514700,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<9.514619,0.000000,54.889700>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<8.514700,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.151856,0.000000,56.139600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.401834,0.000000,56.389581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<10.151856,0.000000,56.139600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.401834,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,56.389581>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<10.401834,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,56.139600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<10.901797,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,56.139600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,55.889619>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<11.151775,0.000000,55.889619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,55.889619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,55.639641>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<10.901797,0.000000,55.639641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,55.639641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.651816,0.000000,55.639641>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,0.000000,0> translate<10.651816,0.000000,55.639641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,55.639641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,55.389659>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<10.901797,0.000000,55.639641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,55.389659>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,55.139678>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,-90.000000,0> translate<11.151775,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.151775,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,54.889700>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<10.901797,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.901797,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.401834,0.000000,54.889700>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<10.401834,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.401834,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<10.151856,0.000000,55.139678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<10.151856,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.789013,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.789013,0.000000,55.139678>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,90.000000,0> translate<11.789013,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.789013,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.038991,0.000000,55.139678>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,0.000000,0> translate<11.789013,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.038991,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.038991,0.000000,54.889700>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,-90.000000,0> translate<12.038991,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.038991,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<11.789013,0.000000,54.889700>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,0.000000,0> translate<11.789013,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.607591,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.607591,0.000000,56.139600>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,90.000000,0> translate<12.607591,0.000000,56.139600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.607591,0.000000,56.139600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.857569,0.000000,56.389581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<12.607591,0.000000,56.139600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.857569,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.357531,0.000000,56.389581>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<12.857569,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.357531,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.607509,0.000000,56.139600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<13.357531,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.607509,0.000000,56.139600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.607509,0.000000,55.139678>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,-90.000000,0> translate<13.607509,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.607509,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.357531,0.000000,54.889700>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<13.357531,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.357531,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.857569,0.000000,54.889700>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<12.857569,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.857569,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.607591,0.000000,55.139678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<12.607591,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<12.607591,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<13.607509,0.000000,56.139600>}
box{<0,0,-0.139700><1.414101,0.035000,0.139700> rotate<0,-44.997120,0> translate<12.607591,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.244666,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.244747,0.000000,56.389581>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<14.244747,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.244747,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.244747,0.000000,55.639641>}
box{<0,0,-0.139700><0.749941,0.035000,0.139700> rotate<0,-90.000000,0> translate<14.244747,0.000000,55.639641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.244747,0.000000,55.639641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.744706,0.000000,55.889619>}
box{<0,0,-0.139700><0.558971,0.035000,0.139700> rotate<0,-26.563155,0> translate<14.244747,0.000000,55.639641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.744706,0.000000,55.889619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.994688,0.000000,55.889619>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,0.000000,0> translate<14.744706,0.000000,55.889619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.994688,0.000000,55.889619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.244666,0.000000,55.639641>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<14.994688,0.000000,55.889619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.244666,0.000000,55.639641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.244666,0.000000,55.139678>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,-90.000000,0> translate<15.244666,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.244666,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.994688,0.000000,54.889700>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<14.994688,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.994688,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.494725,0.000000,54.889700>}
box{<0,0,-0.139700><0.499963,0.035000,0.139700> rotate<0,0.000000,0> translate<14.494725,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.494725,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<14.244747,0.000000,55.139678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<14.244747,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.881903,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.881903,0.000000,55.139678>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,90.000000,0> translate<15.881903,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.881903,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.131881,0.000000,55.139678>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,0.000000,0> translate<15.881903,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.131881,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.131881,0.000000,54.889700>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,-90.000000,0> translate<16.131881,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.131881,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<15.881903,0.000000,54.889700>}
box{<0,0,-0.139700><0.249978,0.035000,0.139700> rotate<0,0.000000,0> translate<15.881903,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.700400,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.700481,0.000000,54.889700>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<16.700481,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.700481,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.700400,0.000000,55.889619>}
box{<0,0,-0.139700><1.414099,0.035000,0.139700> rotate<0,-44.997030,0> translate<16.700481,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.700400,0.000000,55.889619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.700400,0.000000,56.139600>}
box{<0,0,-0.139700><0.249981,0.035000,0.139700> rotate<0,90.000000,0> translate<17.700400,0.000000,56.139600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.700400,0.000000,56.139600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.450422,0.000000,56.389581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<17.450422,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<17.450422,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.950459,0.000000,56.389581>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<16.950459,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.950459,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<16.700481,0.000000,56.139600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<16.700481,0.000000,56.139600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.337637,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.337637,0.000000,56.139600>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,90.000000,0> translate<18.337637,0.000000,56.139600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.337637,0.000000,56.139600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.587616,0.000000,56.389581>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,-44.997388,0> translate<18.337637,0.000000,56.139600> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.587616,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.087578,0.000000,56.389581>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<18.587616,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.087578,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.337556,0.000000,56.139600>}
box{<0,0,-0.139700><0.353525,0.035000,0.139700> rotate<0,44.997388,0> translate<19.087578,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.337556,0.000000,56.139600>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.337556,0.000000,55.139678>}
box{<0,0,-0.139700><0.999922,0.035000,0.139700> rotate<0,-90.000000,0> translate<19.337556,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.337556,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.087578,0.000000,54.889700>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,-44.997030,0> translate<19.087578,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.087578,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.587616,0.000000,54.889700>}
box{<0,0,-0.139700><0.499962,0.035000,0.139700> rotate<0,0.000000,0> translate<18.587616,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.587616,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.337637,0.000000,55.139678>}
box{<0,0,-0.139700><0.353522,0.035000,0.139700> rotate<0,44.997030,0> translate<18.337637,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<18.337637,0.000000,55.139678>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.337556,0.000000,56.139600>}
box{<0,0,-0.139700><1.414101,0.035000,0.139700> rotate<0,-44.997120,0> translate<18.337637,0.000000,55.139678> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.974794,0.000000,55.889619>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.474753,0.000000,56.389581>}
box{<0,0,-0.139700><0.707052,0.035000,0.139700> rotate<0,-44.997209,0> translate<19.974794,0.000000,55.889619> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.474753,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.474753,0.000000,54.889700>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,-90.000000,0> translate<20.474753,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<19.974794,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<20.974713,0.000000,54.889700>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<19.974794,0.000000,54.889700> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<22.361891,0.000000,54.889700>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<22.361891,0.000000,56.389581>}
box{<0,0,-0.139700><1.499881,0.035000,0.139700> rotate<0,90.000000,0> translate<22.361891,0.000000,56.389581> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<22.361891,0.000000,56.389581>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<21.611950,0.000000,55.639641>}
box{<0,0,-0.139700><1.060576,0.035000,0.139700> rotate<0,-44.997030,0> translate<21.611950,0.000000,55.639641> }
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<21.611950,0.000000,55.639641>}
cylinder{<0,0,0><0,0.035000,0>0.139700 translate<22.611869,0.000000,55.639641>}
box{<0,0,-0.139700><0.999919,0.035000,0.139700> rotate<0,0.000000,0> translate<21.611950,0.000000,55.639641> }
//Rect
union{
box{<-5.300000,0,-4.035000><5.300000,0.037000,4.035000> rotate<0,-180.000000,0> translate<59.625000,0.000000,51.435000>}
box{<-2.540000,0,-2.540000><2.540000,0.037000,2.540000> rotate<0,-225.000000,0> translate<43.750000,0.000000,21.500000>}
texture{col_pds}
}
texture{col_wrs}
}
#end
#if(pcb_polygons=on)
union{
//Polygone
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.125000,-1.535000,51.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.125000,-1.535000,53.000000>}
box{<0,0,-0.150000><1.250000,0.035000,0.150000> rotate<0,90.000000,0> translate<4.125000,-1.535000,53.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.125000,-1.535000,51.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.375000,-1.535000,51.750000>}
box{<0,0,-0.150000><4.250000,0.035000,0.150000> rotate<0,0.000000,0> translate<4.125000,-1.535000,51.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<4.125000,-1.535000,53.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.500000,-1.535000,53.000000>}
box{<0,0,-0.150000><4.375000,0.035000,0.150000> rotate<0,0.000000,0> translate<4.125000,-1.535000,53.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.000000,0.000000,24.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.937500,0.000000,24.187500>}
box{<0,0,-0.150000><4.937500,0.035000,0.150000> rotate<0,0.000000,0> translate<6.000000,0.000000,24.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.000000,0.000000,25.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.000000,0.000000,24.187500>}
box{<0,0,-0.150000><1.062500,0.035000,0.150000> rotate<0,-90.000000,0> translate<6.000000,0.000000,24.187500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<6.000000,0.000000,25.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.062500,0.000000,25.250000>}
box{<0,0,-0.150000><3.062500,0.035000,0.150000> rotate<0,0.000000,0> translate<6.000000,0.000000,25.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.375000,-1.535000,51.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.375000,-1.535000,51.750000>}
box{<0,0,-0.150000><0.687500,0.035000,0.150000> rotate<0,90.000000,0> translate<8.375000,-1.535000,51.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.375000,0.000000,51.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.375000,0.000000,53.625000>}
box{<0,0,-0.150000><2.562500,0.035000,0.150000> rotate<0,90.000000,0> translate<8.375000,0.000000,53.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.375000,-1.535000,51.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.937500,-1.535000,51.062500>}
box{<0,0,-0.150000><1.562500,0.035000,0.150000> rotate<0,0.000000,0> translate<8.375000,-1.535000,51.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.375000,0.000000,51.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.500000,0.000000,51.062500>}
box{<0,0,-0.150000><2.125000,0.035000,0.150000> rotate<0,0.000000,0> translate<8.375000,0.000000,51.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.375000,0.000000,53.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.500000,0.000000,53.625000>}
box{<0,0,-0.150000><2.125000,0.035000,0.150000> rotate<0,0.000000,0> translate<8.375000,0.000000,53.625000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.500000,-1.535000,53.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.500000,-1.535000,53.562500>}
box{<0,0,-0.150000><0.562500,0.035000,0.150000> rotate<0,90.000000,0> translate<8.500000,-1.535000,53.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<8.500000,-1.535000,53.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.875000,-1.535000,53.562500>}
box{<0,0,-0.150000><1.375000,0.035000,0.150000> rotate<0,0.000000,0> translate<8.500000,-1.535000,53.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.000000,0.000000,25.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.062500,0.000000,25.250000>}
box{<0,0,-0.150000><0.088388,0.035000,0.150000> rotate<0,44.997030,0> translate<9.000000,0.000000,25.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.000000,0.000000,27.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.000000,0.000000,25.312500>}
box{<0,0,-0.150000><2.437500,0.035000,0.150000> rotate<0,-90.000000,0> translate<9.000000,0.000000,25.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.000000,0.000000,27.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.500000,0.000000,27.750000>}
box{<0,0,-0.150000><1.500000,0.035000,0.150000> rotate<0,0.000000,0> translate<9.000000,0.000000,27.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.125000,0.000000,30.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<13.125000,0.000000,30.375000>}
box{<0,0,-0.150000><4.000000,0.035000,0.150000> rotate<0,0.000000,0> translate<9.125000,0.000000,30.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.125000,0.000000,32.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.125000,0.000000,30.375000>}
box{<0,0,-0.150000><2.125000,0.035000,0.150000> rotate<0,-90.000000,0> translate<9.125000,0.000000,30.375000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.125000,0.000000,32.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<13.125000,0.000000,32.500000>}
box{<0,0,-0.150000><4.000000,0.035000,0.150000> rotate<0,0.000000,0> translate<9.125000,0.000000,32.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.875000,-1.535000,53.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<9.937500,-1.535000,51.062500>}
box{<0,0,-0.150000><2.500781,0.035000,0.150000> rotate<0,88.562059,0> translate<9.875000,-1.535000,53.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.500000,0.000000,25.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.500000,0.000000,27.750000>}
box{<0,0,-0.150000><2.437500,0.035000,0.150000> rotate<0,90.000000,0> translate<10.500000,0.000000,27.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.500000,0.000000,25.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.937500,0.000000,25.312500>}
box{<0,0,-0.150000><0.437500,0.035000,0.150000> rotate<0,0.000000,0> translate<10.500000,0.000000,25.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.500000,0.000000,53.625000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.500000,0.000000,51.062500>}
box{<0,0,-0.150000><2.562500,0.035000,0.150000> rotate<0,-90.000000,0> translate<10.500000,0.000000,51.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.937500,0.000000,24.187500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<10.937500,0.000000,25.312500>}
box{<0,0,-0.150000><1.125000,0.035000,0.150000> rotate<0,90.000000,0> translate<10.937500,0.000000,25.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<13.125000,0.000000,30.375000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<13.125000,0.000000,32.500000>}
box{<0,0,-0.150000><2.125000,0.035000,0.150000> rotate<0,90.000000,0> translate<13.125000,0.000000,32.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.625000,0.000000,51.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.625000,0.000000,52.562500>}
box{<0,0,-0.150000><1.562500,0.035000,0.150000> rotate<0,90.000000,0> translate<14.625000,0.000000,52.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.625000,0.000000,51.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<18.125000,0.000000,51.000000>}
box{<0,0,-0.150000><3.500000,0.035000,0.150000> rotate<0,0.000000,0> translate<14.625000,0.000000,51.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<14.625000,0.000000,52.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<18.125000,0.000000,52.562500>}
box{<0,0,-0.150000><3.500000,0.035000,0.150000> rotate<0,0.000000,0> translate<14.625000,0.000000,52.562500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<18.125000,0.000000,52.562500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<18.125000,0.000000,51.000000>}
box{<0,0,-0.150000><1.562500,0.035000,0.150000> rotate<0,-90.000000,0> translate<18.125000,0.000000,51.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<25.625000,0.000000,52.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<29.500000,0.000000,52.312500>}
box{<0,0,-0.150000><3.875000,0.035000,0.150000> rotate<0,0.000000,0> translate<25.625000,0.000000,52.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<25.625000,0.000000,55.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<25.625000,0.000000,52.312500>}
box{<0,0,-0.150000><3.375000,0.035000,0.150000> rotate<0,-90.000000,0> translate<25.625000,0.000000,52.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<25.625000,0.000000,55.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<29.500000,0.000000,55.687500>}
box{<0,0,-0.150000><3.875000,0.035000,0.150000> rotate<0,0.000000,0> translate<25.625000,0.000000,55.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<29.500000,0.000000,52.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<29.500000,0.000000,55.687500>}
box{<0,0,-0.150000><3.375000,0.035000,0.150000> rotate<0,90.000000,0> translate<29.500000,0.000000,55.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.625000,0.000000,50.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.687500,0.000000,50.062500>}
box{<0,0,-0.150000><0.088388,0.035000,0.150000> rotate<0,-44.997030,0> translate<30.625000,0.000000,50.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.625000,0.000000,50.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.250000,0.000000,50.000000>}
box{<0,0,-0.150000><1.625000,0.035000,0.150000> rotate<0,0.000000,0> translate<30.625000,0.000000,50.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.687500,0.000000,50.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.687500,0.000000,54.437500>}
box{<0,0,-0.150000><4.375000,0.035000,0.150000> rotate<0,90.000000,0> translate<30.687500,0.000000,54.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<30.687500,0.000000,54.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<33.000000,0.000000,54.437500>}
box{<0,0,-0.150000><2.312500,0.035000,0.150000> rotate<0,0.000000,0> translate<30.687500,0.000000,54.437500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.250000,0.000000,51.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.250000,0.000000,50.000000>}
box{<0,0,-0.150000><1.937500,0.035000,0.150000> rotate<0,-90.000000,0> translate<32.250000,0.000000,50.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<32.250000,0.000000,51.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<33.000000,0.000000,52.687500>}
box{<0,0,-0.150000><1.060660,0.035000,0.150000> rotate<0,-44.997030,0> translate<32.250000,0.000000,51.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<33.000000,0.000000,54.437500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<33.000000,0.000000,52.687500>}
box{<0,0,-0.150000><1.750000,0.035000,0.150000> rotate<0,-90.000000,0> translate<33.000000,0.000000,52.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.250000,-1.535000,52.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.250000,-1.535000,54.250000>}
box{<0,0,-0.150000><1.562500,0.035000,0.150000> rotate<0,90.000000,0> translate<44.250000,-1.535000,54.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.250000,-1.535000,52.687500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.750000,-1.535000,52.687500>}
box{<0,0,-0.150000><3.500000,0.035000,0.150000> rotate<0,0.000000,0> translate<44.250000,-1.535000,52.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<44.250000,-1.535000,54.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.687500,-1.535000,54.250000>}
box{<0,0,-0.150000><3.437500,0.035000,0.150000> rotate<0,0.000000,0> translate<44.250000,-1.535000,54.250000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.687500,-1.535000,54.250000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.687500,-1.535000,56.000000>}
box{<0,0,-0.150000><1.750000,0.035000,0.150000> rotate<0,90.000000,0> translate<47.687500,-1.535000,56.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.687500,-1.535000,56.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.687500,-1.535000,56.000000>}
box{<0,0,-0.150000><2.000000,0.035000,0.150000> rotate<0,0.000000,0> translate<47.687500,-1.535000,56.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.750000,-1.535000,50.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.750000,-1.535000,52.687500>}
box{<0,0,-0.150000><2.187500,0.035000,0.150000> rotate<0,90.000000,0> translate<47.750000,-1.535000,52.687500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<47.750000,-1.535000,50.500000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.687500,-1.535000,50.500000>}
box{<0,0,-0.150000><1.937500,0.035000,0.150000> rotate<0,0.000000,0> translate<47.750000,-1.535000,50.500000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.687500,-1.535000,56.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<49.687500,-1.535000,50.500000>}
box{<0,0,-0.150000><5.500000,0.035000,0.150000> rotate<0,-90.000000,0> translate<49.687500,-1.535000,50.500000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.000000,0.000000,57.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.000000,0.000000,59.000000>}
box{<0,0,-0.075000><1.312500,0.035000,0.075000> rotate<0,90.000000,0> translate<54.000000,0.000000,59.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.000000,0.000000,57.687500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<65.562500,0.000000,57.687500>}
box{<0,0,-0.075000><11.562500,0.035000,0.075000> rotate<0,0.000000,0> translate<54.000000,0.000000,57.687500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.000000,0.000000,59.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.062500,0.000000,59.062500>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,-44.997030,0> translate<54.000000,0.000000,59.000000> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<54.062500,0.000000,59.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<65.500000,0.000000,59.062500>}
box{<0,0,-0.075000><11.437500,0.035000,0.075000> rotate<0,0.000000,0> translate<54.062500,0.000000,59.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,0.000000,45.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,0.000000,47.000000>}
box{<0,0,-0.150000><1.250000,0.035000,0.150000> rotate<0,90.000000,0> translate<54.875000,0.000000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,0.000000,45.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.562500,0.000000,45.750000>}
box{<0,0,-0.150000><2.687500,0.035000,0.150000> rotate<0,0.000000,0> translate<54.875000,0.000000,45.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,-1.535000,45.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.312500,-1.535000,45.750000>}
box{<0,0,-0.150000><3.437500,0.035000,0.150000> rotate<0,0.000000,0> translate<54.875000,-1.535000,45.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,-1.535000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,-1.535000,45.750000>}
box{<0,0,-0.150000><1.250000,0.035000,0.150000> rotate<0,-90.000000,0> translate<54.875000,-1.535000,45.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,0.000000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.312500,0.000000,47.000000>}
box{<0,0,-0.150000><3.437500,0.035000,0.150000> rotate<0,0.000000,0> translate<54.875000,0.000000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<54.875000,-1.535000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.312500,-1.535000,47.000000>}
box{<0,0,-0.150000><3.437500,0.035000,0.150000> rotate<0,0.000000,0> translate<54.875000,-1.535000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.187500,0.000000,9.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.187500,0.000000,11.000000>}
box{<0,0,-0.150000><1.687500,0.035000,0.150000> rotate<0,90.000000,0> translate<57.187500,0.000000,11.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.187500,0.000000,9.312500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<59.000000,0.000000,9.312500>}
box{<0,0,-0.150000><1.812500,0.035000,0.150000> rotate<0,0.000000,0> translate<57.187500,0.000000,9.312500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.187500,0.000000,11.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<59.000000,0.000000,11.000000>}
box{<0,0,-0.150000><1.812500,0.035000,0.150000> rotate<0,0.000000,0> translate<57.187500,0.000000,11.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.562500,0.000000,44.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.562500,0.000000,45.750000>}
box{<0,0,-0.150000><0.812500,0.035000,0.150000> rotate<0,90.000000,0> translate<57.562500,0.000000,45.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<57.562500,0.000000,44.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.312500,0.000000,44.937500>}
box{<0,0,-0.150000><0.750000,0.035000,0.150000> rotate<0,0.000000,0> translate<57.562500,0.000000,44.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.312500,-1.535000,45.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.312500,-1.535000,47.000000>}
box{<0,0,-0.150000><1.250000,0.035000,0.150000> rotate<0,90.000000,0> translate<58.312500,-1.535000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.312500,0.000000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<58.312500,0.000000,44.937500>}
box{<0,0,-0.150000><2.062500,0.035000,0.150000> rotate<0,-90.000000,0> translate<58.312500,0.000000,44.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<59.000000,0.000000,11.000000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<59.000000,0.000000,9.312500>}
box{<0,0,-0.150000><1.687500,0.035000,0.150000> rotate<0,-90.000000,0> translate<59.000000,0.000000,9.312500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<59.250000,0.000000,45.562500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<63.937500,0.000000,45.562500>}
box{<0,0,-0.152400><4.687500,0.035000,0.152400> rotate<0,0.000000,0> translate<59.250000,0.000000,45.562500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<59.250000,-1.535000,45.562500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<66.750000,-1.535000,45.562500>}
box{<0,0,-0.152400><7.500000,0.035000,0.152400> rotate<0,0.000000,0> translate<59.250000,-1.535000,45.562500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<59.250000,0.000000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<59.250000,0.000000,45.562500>}
box{<0,0,-0.152400><1.437500,0.035000,0.152400> rotate<0,-90.000000,0> translate<59.250000,0.000000,45.562500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<59.250000,-1.535000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<59.250000,-1.535000,45.562500>}
box{<0,0,-0.152400><1.437500,0.035000,0.152400> rotate<0,-90.000000,0> translate<59.250000,-1.535000,45.562500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<59.250000,0.000000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<63.937500,0.000000,47.000000>}
box{<0,0,-0.152400><4.687500,0.035000,0.152400> rotate<0,0.000000,0> translate<59.250000,0.000000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<59.250000,-1.535000,47.000000>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<65.312500,-1.535000,47.000000>}
box{<0,0,-0.152400><6.062500,0.035000,0.152400> rotate<0,0.000000,0> translate<59.250000,-1.535000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<63.937500,0.000000,45.562500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<63.937500,0.000000,47.000000>}
box{<0,0,-0.152400><1.437500,0.035000,0.152400> rotate<0,90.000000,0> translate<63.937500,0.000000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<65.250000,-1.535000,50.750000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<66.812500,-1.535000,50.875000>}
box{<0,0,-0.150000><1.567492,0.035000,0.150000> rotate<0,-4.573619,0> translate<65.250000,-1.535000,50.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<65.250000,-1.535000,55.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<65.250000,-1.535000,50.750000>}
box{<0,0,-0.150000><4.312500,0.035000,0.150000> rotate<0,-90.000000,0> translate<65.250000,-1.535000,50.750000> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<65.250000,-1.535000,55.062500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<69.375000,-1.535000,55.062500>}
box{<0,0,-0.150000><4.125000,0.035000,0.150000> rotate<0,0.000000,0> translate<65.250000,-1.535000,55.062500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<65.312500,-1.535000,48.062500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<65.312500,-1.535000,47.000000>}
box{<0,0,-0.152400><1.062500,0.035000,0.152400> rotate<0,-90.000000,0> translate<65.312500,-1.535000,47.000000> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<65.312500,-1.535000,48.062500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<66.750000,-1.535000,48.062500>}
box{<0,0,-0.152400><1.437500,0.035000,0.152400> rotate<0,0.000000,0> translate<65.312500,-1.535000,48.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<65.500000,0.000000,59.062500>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<65.562500,0.000000,59.000000>}
box{<0,0,-0.075000><0.088388,0.035000,0.075000> rotate<0,44.997030,0> translate<65.500000,0.000000,59.062500> }
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<65.562500,0.000000,59.000000>}
cylinder{<0,0,0><0,0.035000,0>0.075000 translate<65.562500,0.000000,57.687500>}
box{<0,0,-0.075000><1.312500,0.035000,0.075000> rotate<0,-90.000000,0> translate<65.562500,0.000000,57.687500> }
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<66.750000,-1.535000,45.562500>}
cylinder{<0,0,0><0,0.035000,0>0.152400 translate<66.750000,-1.535000,48.062500>}
box{<0,0,-0.152400><2.500000,0.035000,0.152400> rotate<0,90.000000,0> translate<66.750000,-1.535000,48.062500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<66.812500,-1.535000,50.875000>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<66.812500,-1.535000,52.937500>}
box{<0,0,-0.150000><2.062500,0.035000,0.150000> rotate<0,90.000000,0> translate<66.812500,-1.535000,52.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<66.812500,-1.535000,52.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<69.375000,-1.535000,52.937500>}
box{<0,0,-0.150000><2.562500,0.035000,0.150000> rotate<0,0.000000,0> translate<66.812500,-1.535000,52.937500> }
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<69.375000,-1.535000,52.937500>}
cylinder{<0,0,0><0,0.035000,0>0.150000 translate<69.375000,-1.535000,55.062500>}
box{<0,0,-0.150000><2.125000,0.035000,0.150000> rotate<0,90.000000,0> translate<69.375000,-1.535000,55.062500> }
texture{col_pol}
}
#end
union{
cylinder{<56.400000,0.038000,32.385000><56.400000,-1.538000,32.385000>0.500000}
cylinder{<59.500000,0.038000,37.185000><59.500000,-1.538000,37.185000>0.500000}
cylinder{<62.500000,0.038000,32.385000><62.500000,-1.538000,32.385000>0.500000}
cylinder{<46.875000,0.038000,6.550000><46.875000,-1.538000,6.550000>0.650000}
cylinder{<46.275000,0.038000,1.650000><46.275000,-1.538000,1.650000>0.650000}
cylinder{<61.375000,0.038000,6.550000><61.375000,-1.538000,6.550000>0.650000}
cylinder{<61.975000,0.038000,1.650000><61.975000,-1.538000,1.650000>0.650000}
cylinder{<59.270000,0.038000,20.437500><59.270000,-1.538000,20.437500>0.457200}
cylinder{<56.730000,0.038000,20.437500><56.730000,-1.538000,20.437500>0.457200}
cylinder{<59.270000,0.038000,25.812500><59.270000,-1.538000,25.812500>0.457200}
cylinder{<56.730000,0.038000,25.812500><56.730000,-1.538000,25.812500>0.457200}
cylinder{<31.375000,0.038000,21.480000><31.375000,-1.538000,21.480000>0.457200}
cylinder{<31.375000,0.038000,24.020000><31.375000,-1.538000,24.020000>0.457200}
cylinder{<22.085000,0.038000,21.750000><22.085000,-1.538000,21.750000>0.457200}
cylinder{<24.625000,0.038000,21.750000><24.625000,-1.538000,21.750000>0.457200}
cylinder{<27.165000,0.038000,21.750000><27.165000,-1.538000,21.750000>0.457200}
cylinder{<22.645000,0.038000,3.375000><22.645000,-1.538000,3.375000>0.457200}
cylinder{<20.105000,0.038000,3.375000><20.105000,-1.538000,3.375000>0.457200}
cylinder{<27.165000,0.038000,18.000000><27.165000,-1.538000,18.000000>0.457200}
cylinder{<24.625000,0.038000,18.000000><24.625000,-1.538000,18.000000>0.457200}
cylinder{<22.085000,0.038000,18.000000><22.085000,-1.538000,18.000000>0.457200}
cylinder{<18.770000,0.038000,18.000000><18.770000,-1.538000,18.000000>0.457200}
cylinder{<16.230000,0.038000,18.000000><16.230000,-1.538000,18.000000>0.457200}
cylinder{<18.770000,0.038000,21.750000><18.770000,-1.538000,21.750000>0.457200}
cylinder{<16.230000,0.038000,21.750000><16.230000,-1.538000,21.750000>0.457200}
cylinder{<34.707500,0.038000,2.375000><34.707500,-1.538000,2.375000>0.457200}
cylinder{<32.167500,0.038000,2.375000><32.167500,-1.538000,2.375000>0.457200}
cylinder{<40.332500,0.038000,2.375000><40.332500,-1.538000,2.375000>0.457200}
cylinder{<37.792500,0.038000,2.375000><37.792500,-1.538000,2.375000>0.457200}
cylinder{<29.020000,0.038000,2.312500><29.020000,-1.538000,2.312500>0.457200}
cylinder{<26.480000,0.038000,2.312500><26.480000,-1.538000,2.312500>0.457200}
//Bohrungen(schnell)/Durchkontaktierungen
cylinder{<22.562500,0.038000,43.625000><22.562500,-1.538000,43.625000>0.150000 }
cylinder{<22.312500,0.038000,47.625000><22.312500,-1.538000,47.625000>0.150000 }
cylinder{<22.437500,0.038000,35.125000><22.437500,-1.538000,35.125000>0.150000 }
cylinder{<17.000000,0.038000,41.625000><17.000000,-1.538000,41.625000>0.150000 }
cylinder{<17.062500,0.038000,36.625000><17.062500,-1.538000,36.625000>0.150000 }
cylinder{<37.312500,0.038000,24.437500><37.312500,-1.538000,24.437500>0.150000 }
cylinder{<39.125000,0.038000,26.375000><39.125000,-1.538000,26.375000>0.150000 }
cylinder{<42.062500,0.038000,29.125000><42.062500,-1.538000,29.125000>0.150000 }
cylinder{<49.125000,0.038000,29.562500><49.125000,-1.538000,29.562500>0.150000 }
cylinder{<50.750000,0.038000,23.937500><50.750000,-1.538000,23.937500>0.150000 }
cylinder{<40.375000,0.038000,15.562500><40.375000,-1.538000,15.562500>0.150000 }
cylinder{<36.812500,0.038000,19.062500><36.812500,-1.538000,19.062500>0.150000 }
cylinder{<50.750000,0.038000,19.437500><50.750000,-1.538000,19.437500>0.150000 }
cylinder{<52.500000,0.038000,21.000000><52.500000,-1.538000,21.000000>0.150000 }
cylinder{<14.375000,0.038000,19.500000><14.375000,-1.538000,19.500000>0.150000 }
cylinder{<13.250000,0.038000,21.125000><13.250000,-1.538000,21.125000>0.150000 }
cylinder{<9.125000,0.038000,51.625000><9.125000,-1.538000,51.625000>0.150000 }
cylinder{<47.937500,0.038000,17.062500><47.937500,-1.538000,17.062500>0.150000 }
cylinder{<53.250000,0.038000,17.500000><53.250000,-1.538000,17.500000>0.150000 }
cylinder{<47.062500,0.038000,15.500000><47.062500,-1.538000,15.500000>0.150000 }
cylinder{<45.937500,0.038000,14.375000><45.937500,-1.538000,14.375000>0.150000 }
cylinder{<44.125000,0.038000,12.687500><44.125000,-1.538000,12.687500>0.150000 }
cylinder{<60.125000,0.038000,46.250000><60.125000,-1.538000,46.250000>0.150000 }
cylinder{<61.000000,0.038000,46.250000><61.000000,-1.538000,46.250000>0.150000 }
cylinder{<61.937500,0.038000,46.250000><61.937500,-1.538000,46.250000>0.150000 }
cylinder{<62.875000,0.038000,46.250000><62.875000,-1.538000,46.250000>0.150000 }
cylinder{<58.750000,0.038000,23.687500><58.750000,-1.538000,23.687500>0.150000 }
cylinder{<29.500000,0.038000,24.812500><29.500000,-1.538000,24.812500>0.150000 }
cylinder{<58.937500,0.038000,18.250000><58.937500,-1.538000,18.250000>0.150000 }
cylinder{<67.312500,0.038000,19.562500><67.312500,-1.538000,19.562500>0.150000 }
cylinder{<8.562500,0.038000,52.437500><8.562500,-1.538000,52.437500>0.150000 }
cylinder{<9.187500,0.038000,53.125000><9.187500,-1.538000,53.125000>0.150000 }
cylinder{<30.875000,0.038000,50.375000><30.875000,-1.538000,50.375000>0.150000 }
cylinder{<31.750000,0.038000,50.375000><31.750000,-1.538000,50.375000>0.150000 }
cylinder{<30.875000,0.038000,51.187500><30.875000,-1.538000,51.187500>0.150000 }
cylinder{<31.750000,0.038000,51.187500><31.750000,-1.538000,51.187500>0.150000 }
cylinder{<18.625000,0.038000,4.937500><18.625000,-1.538000,4.937500>0.150000 }
cylinder{<12.750000,0.038000,14.812500><12.750000,-1.538000,14.812500>0.150000 }
cylinder{<17.812500,0.038000,11.812500><17.812500,-1.538000,11.812500>0.150000 }
cylinder{<24.875000,0.038000,4.437500><24.875000,-1.538000,4.437500>0.150000 }
cylinder{<14.125000,0.038000,13.937500><14.125000,-1.538000,13.937500>0.150000 }
cylinder{<23.812500,0.038000,12.562500><23.812500,-1.538000,12.562500>0.150000 }
cylinder{<27.750000,0.038000,9.937500><27.750000,-1.538000,9.937500>0.150000 }
cylinder{<60.062500,0.038000,2.500000><60.062500,-1.538000,2.500000>0.150000 }
cylinder{<60.125000,0.038000,1.312500><60.125000,-1.538000,1.312500>0.150000 }
cylinder{<3.750000,0.038000,16.687500><3.750000,-1.538000,16.687500>0.150000 }
cylinder{<58.562500,0.038000,9.562500><58.562500,-1.538000,9.562500>0.150000 }
cylinder{<15.562500,0.038000,7.187500><15.562500,-1.538000,7.187500>0.150000 }
cylinder{<57.625000,0.038000,9.562500><57.625000,-1.538000,9.562500>0.150000 }
cylinder{<58.500000,0.038000,10.562500><58.500000,-1.538000,10.562500>0.150000 }
cylinder{<57.625000,0.038000,10.562500><57.625000,-1.538000,10.562500>0.150000 }
cylinder{<6.875000,0.038000,6.625000><6.875000,-1.538000,6.625000>0.150000 }
cylinder{<7.750000,0.038000,12.062500><7.750000,-1.538000,12.062500>0.150000 }
cylinder{<57.687500,0.038000,40.937500><57.687500,-1.538000,40.937500>0.150000 }
cylinder{<58.562500,0.038000,40.937500><58.562500,-1.538000,40.937500>0.150000 }
cylinder{<59.437500,0.038000,40.937500><59.437500,-1.538000,40.937500>0.150000 }
cylinder{<60.312500,0.038000,40.937500><60.312500,-1.538000,40.937500>0.150000 }
cylinder{<52.500000,0.038000,47.000000><52.500000,-1.538000,47.000000>0.150000 }
cylinder{<52.500000,0.038000,46.187500><52.500000,-1.538000,46.187500>0.150000 }
cylinder{<52.500000,0.038000,45.375000><52.500000,-1.538000,45.375000>0.150000 }
cylinder{<67.312500,0.038000,21.187500><67.312500,-1.538000,21.187500>0.150000 }
cylinder{<9.562500,0.038000,24.812500><9.562500,-1.538000,24.812500>0.150000 }
cylinder{<10.562500,0.038000,24.812500><10.562500,-1.538000,24.812500>0.150000 }
cylinder{<8.437500,0.038000,24.812500><8.437500,-1.538000,24.812500>0.150000 }
cylinder{<6.437500,0.038000,24.812500><6.437500,-1.538000,24.812500>0.150000 }
cylinder{<7.437500,0.038000,24.812500><7.437500,-1.538000,24.812500>0.150000 }
cylinder{<20.750000,0.038000,9.500000><20.750000,-1.538000,9.500000>0.150000 }
cylinder{<55.312500,0.038000,2.625000><55.312500,-1.538000,2.625000>0.150000 }
cylinder{<19.812500,0.038000,9.687500><19.812500,-1.538000,9.687500>0.150000 }
cylinder{<56.875000,0.038000,3.375000><56.875000,-1.538000,3.375000>0.150000 }
cylinder{<34.687500,0.038000,5.250000><34.687500,-1.538000,5.250000>0.150000 }
cylinder{<24.125000,0.038000,5.000000><24.125000,-1.538000,5.000000>0.150000 }
cylinder{<15.750000,0.038000,3.312500><15.750000,-1.538000,3.312500>0.150000 }
cylinder{<23.937500,0.038000,9.937500><23.937500,-1.538000,9.937500>0.150000 }
cylinder{<22.687500,0.038000,12.562500><22.687500,-1.538000,12.562500>0.150000 }
cylinder{<17.625000,0.038000,10.687500><17.625000,-1.538000,10.687500>0.150000 }
cylinder{<27.500000,0.038000,8.687500><27.500000,-1.538000,8.687500>0.150000 }
cylinder{<6.687500,0.038000,14.437500><6.687500,-1.538000,14.437500>0.150000 }
cylinder{<8.500000,0.038000,15.312500><8.500000,-1.538000,15.312500>0.150000 }
cylinder{<5.875000,0.038000,13.437500><5.875000,-1.538000,13.437500>0.150000 }
cylinder{<15.187500,0.038000,11.750000><15.187500,-1.538000,11.750000>0.150000 }
cylinder{<12.562500,0.038000,11.875000><12.562500,-1.538000,11.875000>0.150000 }
cylinder{<12.312500,0.038000,7.375000><12.312500,-1.538000,7.375000>0.150000 }
cylinder{<10.250000,0.038000,4.812500><10.250000,-1.538000,4.812500>0.150000 }
cylinder{<6.562500,0.038000,1.875000><6.562500,-1.538000,1.875000>0.150000 }
cylinder{<6.562500,0.038000,8.250000><6.562500,-1.538000,8.250000>0.150000 }
cylinder{<3.187500,0.038000,19.125000><3.187500,-1.538000,19.125000>0.150000 }
cylinder{<3.250000,0.038000,22.687500><3.250000,-1.538000,22.687500>0.150000 }
cylinder{<6.062500,0.038000,27.875000><6.062500,-1.538000,27.875000>0.150000 }
cylinder{<3.250000,0.038000,53.875000><3.250000,-1.538000,53.875000>0.150000 }
cylinder{<3.375000,0.038000,49.875000><3.375000,-1.538000,49.875000>0.150000 }
cylinder{<3.375000,0.038000,47.875000><3.375000,-1.538000,47.875000>0.150000 }
cylinder{<3.375000,0.038000,44.875000><3.375000,-1.538000,44.875000>0.150000 }
cylinder{<3.375000,0.038000,41.875000><3.375000,-1.538000,41.875000>0.150000 }
cylinder{<3.312500,0.038000,38.875000><3.312500,-1.538000,38.875000>0.150000 }
cylinder{<3.312500,0.038000,35.875000><3.312500,-1.538000,35.875000>0.150000 }
cylinder{<3.312500,0.038000,32.875000><3.312500,-1.538000,32.875000>0.150000 }
cylinder{<3.125000,0.038000,57.687500><3.125000,-1.538000,57.687500>0.150000 }
cylinder{<27.500000,0.038000,15.250000><27.500000,-1.538000,15.250000>0.150000 }
cylinder{<19.812500,0.038000,41.625000><19.812500,-1.538000,41.625000>0.150000 }
cylinder{<19.562500,0.038000,43.625000><19.562500,-1.538000,43.625000>0.150000 }
cylinder{<20.000000,0.038000,48.625000><20.000000,-1.538000,48.625000>0.150000 }
cylinder{<19.875000,0.038000,36.625000><19.875000,-1.538000,36.625000>0.150000 }
cylinder{<20.062500,0.038000,34.187500><20.062500,-1.538000,34.187500>0.150000 }
cylinder{<24.687500,0.038000,35.500000><24.687500,-1.538000,35.500000>0.150000 }
cylinder{<22.562500,0.038000,37.625000><22.562500,-1.538000,37.625000>0.150000 }
cylinder{<28.500000,0.038000,10.750000><28.500000,-1.538000,10.750000>0.150000 }
cylinder{<38.562500,0.038000,25.750000><38.562500,-1.538000,25.750000>0.150000 }
cylinder{<36.500000,0.038000,23.812500><36.500000,-1.538000,23.812500>0.150000 }
cylinder{<37.437500,0.038000,18.500000><37.437500,-1.538000,18.500000>0.150000 }
cylinder{<41.000000,0.038000,14.937500><41.000000,-1.538000,14.937500>0.150000 }
cylinder{<52.500000,0.038000,21.875000><52.500000,-1.538000,21.875000>0.150000 }
cylinder{<51.625000,0.038000,23.062500><51.625000,-1.538000,23.062500>0.150000 }
cylinder{<53.125000,0.038000,25.500000><53.125000,-1.538000,25.500000>0.150000 }
cylinder{<52.437500,0.038000,25.937500><52.437500,-1.538000,25.937500>0.150000 }
cylinder{<48.437500,0.038000,30.187500><48.437500,-1.538000,30.187500>0.150000 }
cylinder{<42.625000,0.038000,29.875000><42.625000,-1.538000,29.875000>0.150000 }
cylinder{<48.875000,0.038000,17.375000><48.875000,-1.538000,17.375000>0.150000 }
cylinder{<49.812500,0.038000,18.250000><49.812500,-1.538000,18.250000>0.150000 }
cylinder{<54.125000,0.038000,18.500000><54.125000,-1.538000,18.500000>0.150000 }
cylinder{<50.375000,0.038000,5.875000><50.375000,-1.538000,5.875000>0.150000 }
cylinder{<51.875000,0.038000,5.875000><51.875000,-1.538000,5.875000>0.150000 }
cylinder{<53.375000,0.038000,5.875000><53.375000,-1.538000,5.875000>0.150000 }
cylinder{<54.875000,0.038000,5.875000><54.875000,-1.538000,5.875000>0.150000 }
cylinder{<58.125000,0.038000,5.812500><58.125000,-1.538000,5.812500>0.150000 }
cylinder{<26.250000,0.038000,51.250000><26.250000,-1.538000,51.250000>0.150000 }
cylinder{<26.250000,0.038000,27.562500><26.250000,-1.538000,27.562500>0.150000 }
cylinder{<29.625000,0.038000,31.625000><29.625000,-1.538000,31.625000>0.150000 }
cylinder{<29.687500,0.038000,35.000000><29.687500,-1.538000,35.000000>0.150000 }
cylinder{<32.000000,0.038000,49.250000><32.000000,-1.538000,49.250000>0.150000 }
cylinder{<32.000000,0.038000,48.000000><32.000000,-1.538000,48.000000>0.150000 }
cylinder{<17.812500,0.038000,45.875000><17.812500,-1.538000,45.875000>0.150000 }
cylinder{<17.875000,0.038000,44.125000><17.875000,-1.538000,44.125000>0.150000 }
cylinder{<14.937500,0.038000,41.375000><14.937500,-1.538000,41.375000>0.150000 }
cylinder{<17.937500,0.038000,38.437500><17.937500,-1.538000,38.437500>0.150000 }
cylinder{<17.812500,0.038000,34.937500><17.812500,-1.538000,34.937500>0.150000 }
cylinder{<22.500000,0.038000,41.625000><22.500000,-1.538000,41.625000>0.150000 }
cylinder{<22.250000,0.038000,45.625000><22.250000,-1.538000,45.625000>0.150000 }
cylinder{<14.937500,0.038000,34.125000><14.937500,-1.538000,34.125000>0.150000 }
cylinder{<59.625000,0.038000,58.500000><59.625000,-1.538000,58.500000>0.150000 }
cylinder{<60.875000,0.038000,58.500000><60.875000,-1.538000,58.500000>0.150000 }
cylinder{<62.187500,0.038000,58.500000><62.187500,-1.538000,58.500000>0.150000 }
cylinder{<63.437500,0.038000,58.500000><63.437500,-1.538000,58.500000>0.150000 }
cylinder{<64.687500,0.038000,58.500000><64.687500,-1.538000,58.500000>0.150000 }
cylinder{<58.312500,0.038000,58.500000><58.312500,-1.538000,58.500000>0.150000 }
cylinder{<57.062500,0.038000,58.500000><57.062500,-1.538000,58.500000>0.150000 }
cylinder{<55.750000,0.038000,58.500000><55.750000,-1.538000,58.500000>0.150000 }
cylinder{<54.500000,0.038000,58.500000><54.500000,-1.538000,58.500000>0.150000 }
cylinder{<47.937500,0.038000,16.187500><47.937500,-1.538000,16.187500>0.150000 }
cylinder{<46.500000,0.038000,15.000000><46.500000,-1.538000,15.000000>0.150000 }
cylinder{<44.687500,0.038000,11.125000><44.687500,-1.538000,11.125000>0.150000 }
cylinder{<45.125000,0.038000,13.750000><45.125000,-1.538000,13.750000>0.150000 }
cylinder{<67.500000,0.038000,40.875000><67.500000,-1.538000,40.875000>0.150000 }
cylinder{<48.750000,0.038000,55.125000><48.750000,-1.538000,55.125000>0.150000 }
cylinder{<48.750000,0.038000,54.062500><48.750000,-1.538000,54.062500>0.150000 }
cylinder{<48.750000,0.038000,53.062500><48.750000,-1.538000,53.062500>0.150000 }
cylinder{<48.750000,0.038000,52.000000><48.750000,-1.538000,52.000000>0.150000 }
cylinder{<48.750000,0.038000,51.062500><48.750000,-1.538000,51.062500>0.150000 }
cylinder{<55.562500,0.038000,46.437500><55.562500,-1.538000,46.437500>0.150000 }
cylinder{<56.437500,0.038000,46.437500><56.437500,-1.538000,46.437500>0.150000 }
cylinder{<57.312500,0.038000,46.437500><57.312500,-1.538000,46.437500>0.150000 }
cylinder{<68.750000,0.038000,54.062500><68.750000,-1.538000,54.062500>0.150000 }
cylinder{<67.750000,0.038000,54.062500><67.750000,-1.538000,54.062500>0.150000 }
cylinder{<66.750000,0.038000,54.062500><66.750000,-1.538000,54.062500>0.150000 }
cylinder{<65.875000,0.038000,54.062500><65.875000,-1.538000,54.062500>0.150000 }
cylinder{<61.937500,0.038000,20.437500><61.937500,-1.538000,20.437500>0.150000 }
cylinder{<11.375000,0.038000,32.062500><11.375000,-1.538000,32.062500>0.150000 }
cylinder{<12.625000,0.038000,32.062500><12.625000,-1.538000,32.062500>0.150000 }
cylinder{<11.375000,0.038000,30.750000><11.375000,-1.538000,30.750000>0.150000 }
cylinder{<12.625000,0.038000,30.750000><12.625000,-1.538000,30.750000>0.150000 }
cylinder{<16.250000,0.038000,52.062500><16.250000,-1.538000,52.062500>0.150000 }
cylinder{<17.625000,0.038000,52.062500><17.625000,-1.538000,52.062500>0.150000 }
cylinder{<16.937500,0.038000,51.562500><16.937500,-1.538000,51.562500>0.150000 }
cylinder{<26.312500,0.038000,53.062500><26.312500,-1.538000,53.062500>0.150000 }
cylinder{<26.312500,0.038000,54.000000><26.312500,-1.538000,54.000000>0.150000 }
cylinder{<27.250000,0.038000,53.062500><27.250000,-1.538000,53.062500>0.150000 }
cylinder{<26.312500,0.038000,55.000000><26.312500,-1.538000,55.000000>0.150000 }
cylinder{<3.375000,0.038000,50.875000><3.375000,-1.538000,50.875000>0.150000 }
cylinder{<37.375000,0.038000,38.187500><37.375000,-1.538000,38.187500>0.150000 }
cylinder{<50.625000,0.038000,37.750000><50.625000,-1.538000,37.750000>0.150000 }
cylinder{<38.375000,0.038000,37.375000><38.375000,-1.538000,37.375000>0.150000 }
cylinder{<49.187500,0.038000,36.937500><49.187500,-1.538000,36.937500>0.150000 }
cylinder{<37.375000,0.038000,36.562500><37.375000,-1.538000,36.562500>0.150000 }
cylinder{<50.562500,0.038000,36.125000><50.562500,-1.538000,36.125000>0.150000 }
cylinder{<37.812500,0.038000,35.312500><37.812500,-1.538000,35.312500>0.150000 }
cylinder{<49.437500,0.038000,35.312500><49.437500,-1.538000,35.312500>0.150000 }
cylinder{<37.687500,0.038000,32.437500><37.687500,-1.538000,32.437500>0.150000 }
cylinder{<47.875000,0.038000,26.125000><47.875000,-1.538000,26.125000>0.150000 }
cylinder{<36.750000,0.038000,31.500000><36.750000,-1.538000,31.500000>0.150000 }
cylinder{<46.187500,0.038000,26.062500><46.187500,-1.538000,26.062500>0.150000 }
cylinder{<36.375000,0.038000,30.125000><36.375000,-1.538000,30.125000>0.150000 }
cylinder{<44.875000,0.038000,26.125000><44.875000,-1.538000,26.125000>0.150000 }
cylinder{<36.000000,0.038000,28.937500><36.000000,-1.538000,28.937500>0.150000 }
cylinder{<43.687500,0.038000,26.125000><43.687500,-1.538000,26.125000>0.150000 }
cylinder{<37.062500,0.038000,44.812500><37.062500,-1.538000,44.812500>0.150000 }
cylinder{<48.812500,0.038000,44.812500><48.812500,-1.538000,44.812500>0.150000 }
cylinder{<38.125000,0.038000,44.000000><38.125000,-1.538000,44.000000>0.150000 }
cylinder{<47.750000,0.038000,44.000000><47.750000,-1.538000,44.000000>0.150000 }
cylinder{<37.062500,0.038000,43.187500><37.062500,-1.538000,43.187500>0.150000 }
cylinder{<46.687500,0.038000,43.187500><46.687500,-1.538000,43.187500>0.150000 }
cylinder{<38.125000,0.038000,42.375000><38.125000,-1.538000,42.375000>0.150000 }
cylinder{<45.687500,0.038000,42.375000><45.687500,-1.538000,42.375000>0.150000 }
cylinder{<37.187500,0.038000,41.312500><37.187500,-1.538000,41.312500>0.150000 }
cylinder{<44.937500,0.038000,41.312500><44.937500,-1.538000,41.312500>0.150000 }
cylinder{<38.250000,0.038000,40.500000><38.250000,-1.538000,40.500000>0.150000 }
cylinder{<43.875000,0.038000,40.500000><43.875000,-1.538000,40.500000>0.150000 }
cylinder{<37.062500,0.038000,39.687500><37.062500,-1.538000,39.687500>0.150000 }
cylinder{<42.812500,0.038000,39.687500><42.812500,-1.538000,39.687500>0.150000 }
cylinder{<38.375000,0.038000,39.000000><38.375000,-1.538000,39.000000>0.150000 }
cylinder{<41.687500,0.038000,38.937500><41.687500,-1.538000,38.937500>0.150000 }
cylinder{<11.375000,0.038000,27.000000><11.375000,-1.538000,27.000000>0.150000 }
cylinder{<14.437500,0.038000,26.750000><14.437500,-1.538000,26.750000>0.150000 }
cylinder{<12.812500,0.038000,18.437500><12.812500,-1.538000,18.437500>0.150000 }
cylinder{<3.375000,0.038000,48.875000><3.375000,-1.538000,48.875000>0.150000 }
cylinder{<9.562500,0.038000,22.562500><9.562500,-1.538000,22.562500>0.150000 }
cylinder{<2.937500,0.038000,10.562500><2.937500,-1.538000,10.562500>0.150000 }
cylinder{<3.500000,0.038000,9.937500><3.500000,-1.538000,9.937500>0.150000 }
cylinder{<32.812500,0.038000,48.875000><32.812500,-1.538000,48.875000>0.150000 }
cylinder{<32.062500,0.038000,32.437500><32.062500,-1.538000,32.437500>0.150000 }
cylinder{<29.125000,0.038000,32.625000><29.125000,-1.538000,32.625000>0.150000 }
cylinder{<33.562500,0.038000,30.500000><33.562500,-1.538000,30.500000>0.150000 }
cylinder{<29.562500,0.038000,39.312500><29.562500,-1.538000,39.312500>0.150000 }
cylinder{<28.250000,0.038000,39.062500><28.250000,-1.538000,39.062500>0.150000 }
cylinder{<26.812500,0.038000,38.500000><26.812500,-1.538000,38.500000>0.150000 }
cylinder{<27.875000,0.038000,37.812500><27.875000,-1.538000,37.812500>0.150000 }
cylinder{<29.625000,0.038000,37.375000><29.625000,-1.538000,37.375000>0.150000 }
cylinder{<28.500000,0.038000,36.687500><28.500000,-1.538000,36.687500>0.150000 }
cylinder{<25.562500,0.038000,39.937500><25.562500,-1.538000,39.937500>0.150000 }
cylinder{<24.562500,0.038000,39.125000><24.562500,-1.538000,39.125000>0.150000 }
cylinder{<26.375000,0.038000,44.375000><26.375000,-1.538000,44.375000>0.150000 }
cylinder{<28.375000,0.038000,42.937500><28.375000,-1.538000,42.937500>0.150000 }
cylinder{<29.687500,0.038000,42.375000><29.687500,-1.538000,42.375000>0.150000 }
cylinder{<27.750000,0.038000,41.750000><27.750000,-1.538000,41.750000>0.150000 }
cylinder{<29.187500,0.038000,41.187500><29.187500,-1.538000,41.187500>0.150000 }
cylinder{<28.062500,0.038000,40.500000><28.062500,-1.538000,40.500000>0.150000 }
cylinder{<24.500000,0.038000,44.000000><24.500000,-1.538000,44.000000>0.150000 }
cylinder{<25.250000,0.038000,42.937500><25.250000,-1.538000,42.937500>0.150000 }
cylinder{<28.312500,0.038000,48.312500><28.312500,-1.538000,48.312500>0.150000 }
cylinder{<28.875000,0.038000,46.812500><28.875000,-1.538000,46.812500>0.150000 }
cylinder{<27.000000,0.038000,47.250000><27.000000,-1.538000,47.250000>0.150000 }
cylinder{<27.937500,0.038000,45.875000><27.937500,-1.538000,45.875000>0.150000 }
cylinder{<17.500000,0.038000,46.875000><17.500000,-1.538000,46.875000>0.150000 }
cylinder{<29.125000,0.038000,45.250000><29.125000,-1.538000,45.250000>0.150000 }
cylinder{<18.937500,0.038000,46.437500><18.937500,-1.538000,46.437500>0.150000 }
cylinder{<26.437500,0.038000,46.062500><26.437500,-1.538000,46.062500>0.150000 }
cylinder{<29.750000,0.038000,44.125000><29.750000,-1.538000,44.125000>0.150000 }
cylinder{<27.625000,0.038000,44.062500><27.625000,-1.538000,44.062500>0.150000 }
cylinder{<17.625000,0.038000,45.062500><17.625000,-1.538000,45.062500>0.150000 }
cylinder{<25.187500,0.038000,47.187500><25.187500,-1.538000,47.187500>0.150000 }
cylinder{<29.625000,0.038000,33.875000><29.625000,-1.538000,33.875000>0.150000 }
cylinder{<38.250000,0.038000,53.437500><38.250000,-1.538000,53.437500>0.150000 }
cylinder{<28.312500,0.038000,33.875000><28.312500,-1.538000,33.875000>0.150000 }
//Bohrungen(schnell)/Platine
texture{col_hls}
}
#if(pcb_silkscreen=on)
//Bestückungsdruck
union{
//C1 silk screen
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<40.850000,-1.536000,50.207500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<46.550000,-1.536000,50.207500>}
box{<0,0,-0.101600><5.700000,0.036000,0.101600> rotate<0,0.000000,0> translate<40.850000,-1.536000,50.207500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<46.550000,-1.536000,50.207500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<46.550000,-1.536000,52.407500>}
box{<0,0,-0.101600><2.200000,0.036000,0.101600> rotate<0,90.000000,0> translate<46.550000,-1.536000,52.407500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<46.550000,-1.536000,52.407500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<46.550000,-1.536000,54.607500>}
box{<0,0,-0.101600><2.200000,0.036000,0.101600> rotate<0,90.000000,0> translate<46.550000,-1.536000,54.607500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<46.550000,-1.536000,54.607500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<46.550000,-1.536000,56.807500>}
box{<0,0,-0.101600><2.200000,0.036000,0.101600> rotate<0,90.000000,0> translate<46.550000,-1.536000,56.807500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<46.550000,-1.536000,56.807500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<40.850000,-1.536000,56.807500>}
box{<0,0,-0.101600><5.700000,0.036000,0.101600> rotate<0,0.000000,0> translate<40.850000,-1.536000,56.807500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<39.950000,-1.536000,55.907500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<39.950000,-1.536000,54.607500>}
box{<0,0,-0.101600><1.300000,0.036000,0.101600> rotate<0,-90.000000,0> translate<39.950000,-1.536000,54.607500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<39.950000,-1.536000,54.607500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<39.950000,-1.536000,52.407500>}
box{<0,0,-0.101600><2.200000,0.036000,0.101600> rotate<0,-90.000000,0> translate<39.950000,-1.536000,52.407500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<39.950000,-1.536000,52.407500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<39.950000,-1.536000,51.107500>}
box{<0,0,-0.101600><1.300000,0.036000,0.101600> rotate<0,-90.000000,0> translate<39.950000,-1.536000,51.107500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<39.950000,-1.536000,51.107500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<40.850000,-1.536000,50.207500>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,44.997030,0> translate<39.950000,-1.536000,51.107500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<40.850000,-1.536000,56.807500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<39.950000,-1.536000,55.907500>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,-44.997030,0> translate<39.950000,-1.536000,55.907500> }
object{ARC(3.091731,0.203200,197.894848,343.076243,0.036000) translate<43.242162,-1.536000,53.507500>}
object{ARC(3.091622,0.203200,342.590698,377.409302,0.036000) translate<43.250000,-1.536000,53.532500>}
object{ARC(3.091731,0.203200,17.894848,163.076243,0.036000) translate<43.257838,-1.536000,53.507500>}
object{ARC(3.091622,0.203200,162.590698,197.409302,0.036000) translate<43.250000,-1.536000,53.482500>}
//C2 silk screen
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<68.150000,-1.536000,47.312500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<68.150000,-1.536000,50.712500>}
box{<0,0,-0.101600><3.400000,0.036000,0.101600> rotate<0,90.000000,0> translate<68.150000,-1.536000,50.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<68.150000,-1.536000,50.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<67.100000,-1.536000,50.712500>}
box{<0,0,-0.101600><1.050000,0.036000,0.101600> rotate<0,0.000000,0> translate<67.100000,-1.536000,50.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<67.100000,-1.536000,50.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<64.900000,-1.536000,50.712500>}
box{<0,0,-0.101600><2.200000,0.036000,0.101600> rotate<0,0.000000,0> translate<64.900000,-1.536000,50.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<64.900000,-1.536000,50.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<63.850000,-1.536000,50.712500>}
box{<0,0,-0.101600><1.050000,0.036000,0.101600> rotate<0,0.000000,0> translate<63.850000,-1.536000,50.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<63.850000,-1.536000,50.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<63.850000,-1.536000,47.312500>}
box{<0,0,-0.101600><3.400000,0.036000,0.101600> rotate<0,-90.000000,0> translate<63.850000,-1.536000,47.312500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<64.750000,-1.536000,46.412500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<64.900000,-1.536000,46.412500>}
box{<0,0,-0.101600><0.150000,0.036000,0.101600> rotate<0,0.000000,0> translate<64.750000,-1.536000,46.412500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<64.900000,-1.536000,46.412500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<67.050000,-1.536000,46.412500>}
box{<0,0,-0.101600><2.150000,0.036000,0.101600> rotate<0,0.000000,0> translate<64.900000,-1.536000,46.412500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<67.050000,-1.536000,46.412500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<67.250000,-1.536000,46.412500>}
box{<0,0,-0.101600><0.200000,0.036000,0.101600> rotate<0,0.000000,0> translate<67.050000,-1.536000,46.412500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<67.250000,-1.536000,46.412500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<68.150000,-1.536000,47.312500>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,-44.997030,0> translate<67.250000,-1.536000,46.412500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<63.850000,-1.536000,47.312500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<64.750000,-1.536000,46.412500>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,44.997030,0> translate<63.850000,-1.536000,47.312500> }
object{ARC(1.903944,0.203200,299.931512,420.068488,0.036000) translate<66.000000,-1.536000,48.562500>}
object{ARC(1.903944,0.203200,60.068488,119.931512,0.036000) translate<66.000000,-1.536000,48.562500>}
object{ARC(1.903944,0.203200,119.931512,240.068488,0.036000) translate<66.000000,-1.536000,48.562500>}
object{ARC(1.903944,0.203200,240.068488,299.931512,0.036000) translate<66.000000,-1.536000,48.562500>}
//C3 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<53.276000,-1.536000,21.745000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<53.276000,-1.536000,21.255000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<53.276000,-1.536000,21.255000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<53.724000,-1.536000,21.255000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<53.724000,-1.536000,21.745000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<53.724000,-1.536000,21.745000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<53.504850,-1.536000,21.904000>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<53.504850,-1.536000,21.091200>}
//C4 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<47.577350,-1.536000,18.331631>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<47.230869,-1.536000,17.985150>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<47.230869,-1.536000,17.985150> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<47.547653,-1.536000,17.668366>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<47.894134,-1.536000,18.014847>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<47.547653,-1.536000,17.668366> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-225.000000,0> translate<47.851603,-1.536000,18.282241>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-225.000000,0> translate<47.276866,-1.536000,17.707503>}
//C6 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<43.894134,-1.536000,10.860150>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<43.547653,-1.536000,11.206631>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<43.547653,-1.536000,11.206631> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<43.230869,-1.536000,10.889847>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<43.577350,-1.536000,10.543366>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<43.230869,-1.536000,10.889847> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-135.000000,0> translate<43.844744,-1.536000,10.585897>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-135.000000,0> translate<43.270006,-1.536000,11.160634>}
//C7 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.276000,0.000000,12.067500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.276000,0.000000,12.557500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<28.276000,0.000000,12.557500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.724000,0.000000,12.557500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.724000,0.000000,12.067500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<28.724000,0.000000,12.067500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<28.504850,0.000000,11.908500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<28.504850,0.000000,12.721300>}
//C8 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.724000,0.000000,14.807500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.724000,0.000000,14.317500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<28.724000,0.000000,14.317500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.276000,0.000000,14.317500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.276000,0.000000,14.807500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<28.276000,0.000000,14.807500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<28.495150,0.000000,14.966500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<28.495150,0.000000,14.153700>}
//C9 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.724000,0.000000,9.557500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.724000,0.000000,9.067500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<26.724000,0.000000,9.067500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.276000,0.000000,9.067500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.276000,0.000000,9.557500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<26.276000,0.000000,9.557500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<26.495150,0.000000,9.716500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<26.495150,0.000000,8.903700>}
//C10 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.276000,-1.536000,9.182500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.276000,-1.536000,8.692500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<17.276000,-1.536000,8.692500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.724000,-1.536000,8.692500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.724000,-1.536000,9.182500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<17.724000,-1.536000,9.182500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<17.504850,-1.536000,9.341500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<17.504850,-1.536000,8.528700>}
//C11 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<7.036500,-1.536000,12.567500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<7.036500,-1.536000,13.057500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<7.036500,-1.536000,13.057500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<6.588500,-1.536000,13.057500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<6.588500,-1.536000,12.567500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<6.588500,-1.536000,12.567500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<6.807650,-1.536000,12.408500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<6.807650,-1.536000,13.221300>}
//C12 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<8.849000,-1.536000,13.567500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<8.849000,-1.536000,14.057500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<8.849000,-1.536000,14.057500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<8.401000,-1.536000,14.057500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<8.401000,-1.536000,13.567500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<8.401000,-1.536000,13.567500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<8.620150,-1.536000,13.408500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<8.620150,-1.536000,14.221300>}
//C13 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<50.831634,-1.536000,22.797650>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<50.485153,-1.536000,23.144131>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<50.485153,-1.536000,23.144131> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<50.168369,-1.536000,22.827347>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<50.514850,-1.536000,22.480866>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<50.168369,-1.536000,22.827347> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-135.000000,0> translate<50.782244,-1.536000,22.523397>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-135.000000,0> translate<50.207506,-1.536000,23.098134>}
//C14 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.831634,-1.536000,15.735150>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.485153,-1.536000,16.081631>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<41.485153,-1.536000,16.081631> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.168369,-1.536000,15.764847>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.514850,-1.536000,15.418366>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<41.168369,-1.536000,15.764847> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-135.000000,0> translate<41.782244,-1.536000,15.460897>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-135.000000,0> translate<41.207506,-1.536000,16.035634>}
//C15 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.807500,0.000000,42.276000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.317500,0.000000,42.276000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.317500,0.000000,42.276000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.317500,0.000000,42.724000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.807500,0.000000,42.724000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.317500,0.000000,42.724000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<18.966500,0.000000,42.504850>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<18.153700,0.000000,42.504850>}
//C16 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.692500,0.000000,44.599000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.182500,0.000000,44.599000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.692500,0.000000,44.599000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.182500,0.000000,44.151000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.692500,0.000000,44.151000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.692500,0.000000,44.151000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<20.533500,0.000000,44.370150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<21.346300,0.000000,44.370150>}
//C17 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.692500,0.000000,43.099000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.182500,0.000000,43.099000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.692500,0.000000,43.099000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.182500,0.000000,42.651000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.692500,0.000000,42.651000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.692500,0.000000,42.651000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<20.533500,0.000000,42.870150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<21.346300,0.000000,42.870150>}
//C18 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.807500,0.000000,40.526000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.317500,0.000000,40.526000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.317500,0.000000,40.526000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.317500,0.000000,40.974000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.807500,0.000000,40.974000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.317500,0.000000,40.974000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<18.966500,0.000000,40.754850>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<18.153700,0.000000,40.754850>}
//C19 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.682500,0.000000,37.151000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.192500,0.000000,37.151000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.192500,0.000000,37.151000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.192500,0.000000,37.599000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.682500,0.000000,37.599000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.192500,0.000000,37.599000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<18.841500,0.000000,37.379850>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<18.028700,0.000000,37.379850>}
//C20 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.682500,0.000000,35.651000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.192500,0.000000,35.651000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.192500,0.000000,35.651000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.192500,0.000000,36.099000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.682500,0.000000,36.099000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.192500,0.000000,36.099000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<18.841500,0.000000,35.879850>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<18.028700,0.000000,35.879850>}
//C21 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.505000,0.000000,35.349000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.995000,0.000000,35.349000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.505000,0.000000,35.349000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.995000,0.000000,34.901000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.505000,0.000000,34.901000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.505000,0.000000,34.901000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<20.346000,0.000000,35.120150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<21.158800,0.000000,35.120150>}
//C22 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.442500,0.000000,47.849000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.932500,0.000000,47.849000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.442500,0.000000,47.849000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.932500,0.000000,47.401000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.442500,0.000000,47.401000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.442500,0.000000,47.401000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<20.283500,0.000000,47.620150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<21.096300,0.000000,47.620150>}
//C23 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.349000,0.000000,11.932500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.349000,0.000000,11.442500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<27.349000,0.000000,11.442500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.901000,0.000000,11.442500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.901000,0.000000,11.932500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<26.901000,0.000000,11.932500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<27.120150,0.000000,12.091500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<27.120150,0.000000,11.278700>}
//C24 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.349000,0.000000,12.682500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.349000,0.000000,12.192500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<14.349000,0.000000,12.192500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<13.901000,0.000000,12.192500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<13.901000,0.000000,12.682500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<13.901000,0.000000,12.682500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<14.120150,0.000000,12.841500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<14.120150,0.000000,12.028700>}
//C25 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<46.047653,-1.536000,15.730866>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<46.394134,-1.536000,16.077347>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<46.047653,-1.536000,15.730866> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<46.077350,-1.536000,16.394131>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<45.730869,-1.536000,16.047650>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<45.730869,-1.536000,16.047650> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<45.773400,-1.536000,15.780256>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<46.348137,-1.536000,16.354994>}
//C26 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<44.360153,-1.536000,14.043366>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<44.706634,-1.536000,14.389847>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<44.360153,-1.536000,14.043366> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<44.389850,-1.536000,14.706631>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<44.043369,-1.536000,14.360150>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<44.043369,-1.536000,14.360150> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<44.085900,-1.536000,14.092756>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<44.660637,-1.536000,14.667494>}
//C27 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<43.764850,-1.536000,29.706631>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<43.418369,-1.536000,29.360150>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<43.418369,-1.536000,29.360150> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<43.735153,-1.536000,29.043366>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<44.081634,-1.536000,29.389847>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<43.735153,-1.536000,29.043366> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-225.000000,0> translate<44.039103,-1.536000,29.657241>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-225.000000,0> translate<43.464366,-1.536000,29.082503>}
//C28 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<49.168369,-1.536000,30.577347>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<49.514850,-1.536000,30.230866>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<49.168369,-1.536000,30.577347> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<49.831634,-1.536000,30.547650>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<49.485153,-1.536000,30.894131>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<49.485153,-1.536000,30.894131> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-315.000000,0> translate<49.217759,-1.536000,30.851600>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-315.000000,0> translate<49.792497,-1.536000,30.276863>}
//C29 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<54.264850,-1.536000,17.519131>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<53.918369,-1.536000,17.172650>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<53.918369,-1.536000,17.172650> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<54.235153,-1.536000,16.855866>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<54.581634,-1.536000,17.202347>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<54.235153,-1.536000,16.855866> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-225.000000,0> translate<54.539103,-1.536000,17.469741>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-225.000000,0> translate<53.964366,-1.536000,16.895003>}
//C30 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<49.422653,-1.536000,19.168366>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<49.769134,-1.536000,19.514847>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<49.422653,-1.536000,19.168366> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<49.452350,-1.536000,19.831631>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<49.105869,-1.536000,19.485150>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<49.105869,-1.536000,19.485150> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<49.148400,-1.536000,19.217756>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<49.723137,-1.536000,19.792494>}
//C31 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.360153,-1.536000,24.918366>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.706634,-1.536000,25.264847>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<39.360153,-1.536000,24.918366> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.389850,-1.536000,25.581631>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.043369,-1.536000,25.235150>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<39.043369,-1.536000,25.235150> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<39.085900,-1.536000,24.967756>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<39.660637,-1.536000,25.542494>}
//C32 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.735153,-1.536000,23.230866>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<38.081634,-1.536000,23.577347>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<37.735153,-1.536000,23.230866> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.764850,-1.536000,23.894131>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.418369,-1.536000,23.547650>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,-44.997030,0> translate<37.418369,-1.536000,23.547650> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<37.460900,-1.536000,23.280256>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-45.000000,0> translate<38.035637,-1.536000,23.854994>}
//C33 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<38.269134,-1.536000,19.360150>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.922653,-1.536000,19.706631>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<37.922653,-1.536000,19.706631> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.605869,-1.536000,19.389847>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.952350,-1.536000,19.043366>}
box{<0,0,-0.076200><0.489998,0.036000,0.076200> rotate<0,44.997030,0> translate<37.605869,-1.536000,19.389847> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-135.000000,0> translate<38.219744,-1.536000,19.085897>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-135.000000,0> translate<37.645006,-1.536000,19.660634>}
//C34 silk screen
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<7.600000,0.000000,28.312500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<7.600000,0.000000,31.712500>}
box{<0,0,-0.101600><3.400000,0.036000,0.101600> rotate<0,90.000000,0> translate<7.600000,0.000000,31.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<7.600000,0.000000,31.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<8.650000,0.000000,31.712500>}
box{<0,0,-0.101600><1.050000,0.036000,0.101600> rotate<0,0.000000,0> translate<7.600000,0.000000,31.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<8.650000,0.000000,31.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.850000,0.000000,31.712500>}
box{<0,0,-0.101600><2.200000,0.036000,0.101600> rotate<0,0.000000,0> translate<8.650000,0.000000,31.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.850000,0.000000,31.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.900000,0.000000,31.712500>}
box{<0,0,-0.101600><1.050000,0.036000,0.101600> rotate<0,0.000000,0> translate<10.850000,0.000000,31.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.900000,0.000000,31.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.900000,0.000000,28.312500>}
box{<0,0,-0.101600><3.400000,0.036000,0.101600> rotate<0,-90.000000,0> translate<11.900000,0.000000,28.312500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.000000,0.000000,27.412500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.850000,0.000000,27.412500>}
box{<0,0,-0.101600><0.150000,0.036000,0.101600> rotate<0,0.000000,0> translate<10.850000,0.000000,27.412500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.850000,0.000000,27.412500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<8.700000,0.000000,27.412500>}
box{<0,0,-0.101600><2.150000,0.036000,0.101600> rotate<0,0.000000,0> translate<8.700000,0.000000,27.412500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<8.700000,0.000000,27.412500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<8.500000,0.000000,27.412500>}
box{<0,0,-0.101600><0.200000,0.036000,0.101600> rotate<0,0.000000,0> translate<8.500000,0.000000,27.412500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<8.500000,0.000000,27.412500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<7.600000,0.000000,28.312500>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,44.997030,0> translate<7.600000,0.000000,28.312500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.900000,0.000000,28.312500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.000000,0.000000,27.412500>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,-44.997030,0> translate<11.000000,0.000000,27.412500> }
object{ARC(1.903944,0.203200,119.931512,240.068488,0.036000) translate<9.750000,0.000000,29.562500>}
object{ARC(1.903944,0.203200,60.068488,119.931512,0.036000) translate<9.750000,0.000000,29.562500>}
object{ARC(1.903944,0.203200,299.931512,420.068488,0.036000) translate<9.750000,0.000000,29.562500>}
object{ARC(1.903944,0.203200,240.068488,299.931512,0.036000) translate<9.750000,0.000000,29.562500>}
//C35 silk screen
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<31.312500,0.000000,52.662500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<27.912500,0.000000,52.662500>}
box{<0,0,-0.101600><3.400000,0.036000,0.101600> rotate<0,0.000000,0> translate<27.912500,0.000000,52.662500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<27.912500,0.000000,52.662500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<27.912500,0.000000,53.712500>}
box{<0,0,-0.101600><1.050000,0.036000,0.101600> rotate<0,90.000000,0> translate<27.912500,0.000000,53.712500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<27.912500,0.000000,53.712500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<27.912500,0.000000,55.912500>}
box{<0,0,-0.101600><2.200000,0.036000,0.101600> rotate<0,90.000000,0> translate<27.912500,0.000000,55.912500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<27.912500,0.000000,55.912500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<27.912500,0.000000,56.962500>}
box{<0,0,-0.101600><1.050000,0.036000,0.101600> rotate<0,90.000000,0> translate<27.912500,0.000000,56.962500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<27.912500,0.000000,56.962500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<31.312500,0.000000,56.962500>}
box{<0,0,-0.101600><3.400000,0.036000,0.101600> rotate<0,0.000000,0> translate<27.912500,0.000000,56.962500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<32.212500,0.000000,56.062500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<32.212500,0.000000,55.912500>}
box{<0,0,-0.101600><0.150000,0.036000,0.101600> rotate<0,-90.000000,0> translate<32.212500,0.000000,55.912500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<32.212500,0.000000,55.912500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<32.212500,0.000000,53.762500>}
box{<0,0,-0.101600><2.150000,0.036000,0.101600> rotate<0,-90.000000,0> translate<32.212500,0.000000,53.762500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<32.212500,0.000000,53.762500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<32.212500,0.000000,53.562500>}
box{<0,0,-0.101600><0.200000,0.036000,0.101600> rotate<0,-90.000000,0> translate<32.212500,0.000000,53.562500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<32.212500,0.000000,53.562500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<31.312500,0.000000,52.662500>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,-44.997030,0> translate<31.312500,0.000000,52.662500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<31.312500,0.000000,56.962500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<32.212500,0.000000,56.062500>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,44.997030,0> translate<31.312500,0.000000,56.962500> }
object{ARC(1.903944,0.203200,209.931512,330.068488,0.036000) translate<30.062500,0.000000,54.812500>}
object{ARC(1.903944,0.203200,150.068488,209.931512,0.036000) translate<30.062500,0.000000,54.812500>}
object{ARC(1.903944,0.203200,29.931512,150.068488,0.036000) translate<30.062500,0.000000,54.812500>}
object{ARC(1.903944,0.203200,330.068488,389.931512,0.036000) translate<30.062500,0.000000,54.812500>}
//C36 silk screen
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.437500,0.000000,53.900000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.837500,0.000000,53.900000>}
box{<0,0,-0.101600><3.400000,0.036000,0.101600> rotate<0,0.000000,0> translate<11.437500,0.000000,53.900000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.837500,0.000000,53.900000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.837500,0.000000,52.850000>}
box{<0,0,-0.101600><1.050000,0.036000,0.101600> rotate<0,-90.000000,0> translate<14.837500,0.000000,52.850000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.837500,0.000000,52.850000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.837500,0.000000,50.650000>}
box{<0,0,-0.101600><2.200000,0.036000,0.101600> rotate<0,-90.000000,0> translate<14.837500,0.000000,50.650000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.837500,0.000000,50.650000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.837500,0.000000,49.600000>}
box{<0,0,-0.101600><1.050000,0.036000,0.101600> rotate<0,-90.000000,0> translate<14.837500,0.000000,49.600000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.837500,0.000000,49.600000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.437500,0.000000,49.600000>}
box{<0,0,-0.101600><3.400000,0.036000,0.101600> rotate<0,0.000000,0> translate<11.437500,0.000000,49.600000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.537500,0.000000,50.500000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.537500,0.000000,50.650000>}
box{<0,0,-0.101600><0.150000,0.036000,0.101600> rotate<0,90.000000,0> translate<10.537500,0.000000,50.650000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.537500,0.000000,50.650000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.537500,0.000000,52.800000>}
box{<0,0,-0.101600><2.150000,0.036000,0.101600> rotate<0,90.000000,0> translate<10.537500,0.000000,52.800000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.537500,0.000000,52.800000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.537500,0.000000,53.000000>}
box{<0,0,-0.101600><0.200000,0.036000,0.101600> rotate<0,90.000000,0> translate<10.537500,0.000000,53.000000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.537500,0.000000,53.000000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.437500,0.000000,53.900000>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,-44.997030,0> translate<10.537500,0.000000,53.000000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<11.437500,0.000000,49.600000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<10.537500,0.000000,50.500000>}
box{<0,0,-0.101600><1.272792,0.036000,0.101600> rotate<0,44.997030,0> translate<10.537500,0.000000,50.500000> }
object{ARC(1.903944,0.203200,29.931512,150.068488,0.036000) translate<12.687500,0.000000,51.750000>}
object{ARC(1.903944,0.203200,330.068488,389.931512,0.036000) translate<12.687500,0.000000,51.750000>}
object{ARC(1.903944,0.203200,209.931512,330.068488,0.036000) translate<12.687500,0.000000,51.750000>}
object{ARC(1.903944,0.203200,150.068488,209.931512,0.036000) translate<12.687500,0.000000,51.750000>}
//CON1 silk screen
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<69.900000,0.000000,28.085000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<69.900000,0.000000,36.685000>}
box{<0,0,-0.101600><8.600000,0.036000,0.101600> rotate<0,90.000000,0> translate<69.900000,0.000000,36.685000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<69.900000,0.000000,36.685000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,36.685000>}
box{<0,0,-0.101600><14.300000,0.036000,0.101600> rotate<0,0.000000,0> translate<55.600000,0.000000,36.685000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,36.685000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,28.085000>}
box{<0,0,-0.101600><8.600000,0.036000,0.101600> rotate<0,-90.000000,0> translate<55.600000,0.000000,28.085000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,28.085000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<69.900000,0.000000,28.085000>}
box{<0,0,-0.101600><14.300000,0.036000,0.101600> rotate<0,0.000000,0> translate<55.600000,0.000000,28.085000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,29.685000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,28.085000>}
box{<0,0,-0.101600><1.600000,0.036000,0.101600> rotate<0,-90.000000,0> translate<55.600000,0.000000,28.085000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,36.685000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,35.085000>}
box{<0,0,-0.101600><1.600000,0.036000,0.101600> rotate<0,-90.000000,0> translate<55.600000,0.000000,35.085000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<69.775000,0.000000,29.385000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<62.500000,0.000000,29.385000>}
box{<0,0,-0.101600><7.275000,0.036000,0.101600> rotate<0,0.000000,0> translate<62.500000,0.000000,29.385000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<69.800000,0.000000,35.385000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<62.500000,0.000000,35.385000>}
box{<0,0,-0.101600><7.300000,0.036000,0.101600> rotate<0,0.000000,0> translate<62.500000,0.000000,35.385000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<62.500000,0.000000,35.385000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<62.500000,0.000000,29.385000>}
box{<0,0,-0.101600><6.000000,0.036000,0.101600> rotate<0,-90.000000,0> translate<62.500000,0.000000,29.385000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<68.500000,0.000000,31.485000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<64.000000,0.000000,31.485000>}
box{<0,0,-0.101600><4.500000,0.036000,0.101600> rotate<0,0.000000,0> translate<64.000000,0.000000,31.485000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<68.500000,0.000000,33.285000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<64.000000,0.000000,33.285000>}
box{<0,0,-0.101600><4.500000,0.036000,0.101600> rotate<0,0.000000,0> translate<64.000000,0.000000,33.285000> }
object{ARC(0.906209,0.203200,276.710945,443.289055,0.036000) translate<68.394100,0.000000,32.385000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<69.900000,0.000000,36.685000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<61.950000,0.000000,36.685000>}
box{<0,0,-0.101600><7.950000,0.036000,0.101600> rotate<0,0.000000,0> translate<61.950000,0.000000,36.685000> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<57.200000,0.000000,36.685000>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<55.600000,0.000000,36.685000>}
box{<0,0,-0.101600><1.600000,0.036000,0.101600> rotate<0,0.000000,0> translate<55.600000,0.000000,36.685000> }
//CON2 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<61.375000,0.000000,7.250000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<46.875000,0.000000,7.250000>}
box{<0,0,-0.063500><14.500000,0.036000,0.063500> rotate<0,0.000000,0> translate<46.875000,0.000000,7.250000> }
//CON3 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<30.437500,0.000000,51.375000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<30.437500,0.000000,27.375000>}
box{<0,0,-0.063500><24.000000,0.036000,0.063500> rotate<0,-90.000000,0> translate<30.437500,0.000000,27.375000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<30.437500,0.000000,27.375000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,27.375000>}
box{<0,0,-0.063500><3.500000,0.036000,0.063500> rotate<0,0.000000,0> translate<26.937500,0.000000,27.375000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,27.375000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,26.875000>}
box{<0,0,-0.063500><0.500000,0.036000,0.063500> rotate<0,-90.000000,0> translate<26.937500,0.000000,26.875000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,26.875000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<25.937500,0.000000,26.875000>}
box{<0,0,-0.063500><1.000000,0.036000,0.063500> rotate<0,0.000000,0> translate<25.937500,0.000000,26.875000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<25.937500,0.000000,26.875000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<25.937500,0.000000,51.875000>}
box{<0,0,-0.063500><25.000000,0.036000,0.063500> rotate<0,90.000000,0> translate<25.937500,0.000000,51.875000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<25.937500,0.000000,51.875000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,51.875000>}
box{<0,0,-0.063500><1.000000,0.036000,0.063500> rotate<0,0.000000,0> translate<25.937500,0.000000,51.875000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,51.875000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,51.375000>}
box{<0,0,-0.063500><0.500000,0.036000,0.063500> rotate<0,-90.000000,0> translate<26.937500,0.000000,51.375000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,51.375000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<30.437500,0.000000,51.375000>}
box{<0,0,-0.063500><3.500000,0.036000,0.063500> rotate<0,0.000000,0> translate<26.937500,0.000000,51.375000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,51.375000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<26.937500,0.000000,27.375000>}
box{<0,0,-0.063500><24.000000,0.036000,0.063500> rotate<0,-90.000000,0> translate<26.937500,0.000000,27.375000> }
//CON4 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<5.637500,0.000000,8.162500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<13.437500,0.000000,8.162500>}
box{<0,0,-0.063500><7.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<5.637500,0.000000,8.162500> }
//CON5 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<4.312500,0.000000,23.162500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<4.312500,0.000000,13.962500>}
box{<0,0,-0.063500><9.200000,0.036000,0.063500> rotate<0,-90.000000,0> translate<4.312500,0.000000,13.962500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<4.312500,0.000000,13.962500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<0.212500,0.000000,13.962500>}
box{<0,0,-0.063500><4.100000,0.036000,0.063500> rotate<0,0.000000,0> translate<0.212500,0.000000,13.962500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<0.212500,0.000000,13.962500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<0.212500,0.000000,23.162500>}
box{<0,0,-0.063500><9.200000,0.036000,0.063500> rotate<0,90.000000,0> translate<0.212500,0.000000,23.162500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<0.212500,0.000000,23.162500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<4.312500,0.000000,23.162500>}
box{<0,0,-0.063500><4.100000,0.036000,0.063500> rotate<0,0.000000,0> translate<0.212500,0.000000,23.162500> }
//CON6 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<3.962500,-1.536000,19.700000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<0.062500,-1.536000,19.700000>}
box{<0,0,-0.063500><3.900000,0.036000,0.063500> rotate<0,0.000000,0> translate<0.062500,-1.536000,19.700000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<0.062500,-1.536000,19.700000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<0.062500,-1.536000,57.050000>}
box{<0,0,-0.063500><37.350000,0.036000,0.063500> rotate<0,90.000000,0> translate<0.062500,-1.536000,57.050000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<0.062500,-1.536000,57.050000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<3.962500,-1.536000,57.050000>}
box{<0,0,-0.063500><3.900000,0.036000,0.063500> rotate<0,0.000000,0> translate<0.062500,-1.536000,57.050000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<3.962500,-1.536000,57.050000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<3.962500,-1.536000,19.700000>}
box{<0,0,-0.063500><37.350000,0.036000,0.063500> rotate<0,-90.000000,0> translate<3.962500,-1.536000,19.700000> }
//IC2 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<65.125000,0.000000,56.400000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.025000,0.000000,56.400000>}
box{<0,0,-0.063500><11.100000,0.036000,0.063500> rotate<0,0.000000,0> translate<54.025000,0.000000,56.400000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.025000,0.000000,56.400000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.025000,0.000000,45.800000>}
box{<0,0,-0.063500><10.600000,0.036000,0.063500> rotate<0,-90.000000,0> translate<54.025000,0.000000,45.800000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.025000,0.000000,45.800000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<65.125000,0.000000,45.800000>}
box{<0,0,-0.063500><11.100000,0.036000,0.063500> rotate<0,0.000000,0> translate<54.025000,0.000000,45.800000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<65.125000,0.000000,45.800000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<65.125000,0.000000,56.400000>}
box{<0,0,-0.063500><10.600000,0.036000,0.063500> rotate<0,90.000000,0> translate<65.125000,0.000000,56.400000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<64.825000,0.000000,55.900000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.325000,0.000000,55.900000>}
box{<0,0,-0.063500><10.500000,0.036000,0.063500> rotate<0,0.000000,0> translate<54.325000,0.000000,55.900000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.325000,0.000000,55.900000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.325000,0.000000,46.100000>}
box{<0,0,-0.063500><9.800000,0.036000,0.063500> rotate<0,-90.000000,0> translate<54.325000,0.000000,46.100000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.325000,0.000000,46.100000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<64.825000,0.000000,46.100000>}
box{<0,0,-0.063500><10.500000,0.036000,0.063500> rotate<0,0.000000,0> translate<54.325000,0.000000,46.100000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<64.825000,0.000000,46.100000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<64.825000,0.000000,55.900000>}
box{<0,0,-0.063500><9.800000,0.036000,0.063500> rotate<0,90.000000,0> translate<64.825000,0.000000,55.900000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<64.825000,0.000000,55.900000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<65.125000,0.000000,56.400000>}
box{<0,0,-0.063500><0.583095,0.036000,0.063500> rotate<0,-59.032347,0> translate<64.825000,0.000000,55.900000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.325000,0.000000,55.900000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.025000,0.000000,56.400000>}
box{<0,0,-0.063500><0.583095,0.036000,0.063500> rotate<0,59.032347,0> translate<54.025000,0.000000,56.400000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.325000,0.000000,46.100000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<54.025000,0.000000,45.800000>}
box{<0,0,-0.063500><0.424264,0.036000,0.063500> rotate<0,-44.997030,0> translate<54.025000,0.000000,45.800000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<64.825000,0.000000,46.100000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<65.125000,0.000000,45.800000>}
box{<0,0,-0.063500><0.424264,0.036000,0.063500> rotate<0,44.997030,0> translate<64.825000,0.000000,46.100000> }
box{<-0.300000,0,-1.050000><0.300000,0.036000,1.050000> rotate<0,-180.000000,0> translate<63.025000,0.000000,44.750000>}
box{<-0.300000,0,-1.050000><0.300000,0.036000,1.050000> rotate<0,-180.000000,0> translate<61.325000,0.000000,44.750000>}
box{<-0.300000,0,-1.050000><0.300000,0.036000,1.050000> rotate<0,-180.000000,0> translate<59.625000,0.000000,44.750000>}
box{<-0.300000,0,-1.050000><0.300000,0.036000,1.050000> rotate<0,-180.000000,0> translate<57.925000,0.000000,44.750000>}
box{<-0.300000,0,-1.050000><0.300000,0.036000,1.050000> rotate<0,-180.000000,0> translate<56.225000,0.000000,44.750000>}
//IC4 silk screen
object{ARC(0.250000,0.152400,180.000000,270.000000,0.036000) translate<18.400000,-1.536000,8.812500>}
object{ARC(0.250000,0.152400,270.000000,360.000000,0.036000) translate<23.100000,-1.536000,8.812500>}
object{ARC(0.250000,0.152400,0.000000,90.000000,0.036000) translate<23.100000,-1.536000,13.312500>}
object{ARC(0.250000,0.152400,90.000000,180.000000,0.036000) translate<18.400000,-1.536000,13.312500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.110000,-1.536000,13.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.410000,-1.536000,13.562500>}
box{<0,0,-0.076200><4.700000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.410000,-1.536000,13.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.410000,-1.536000,8.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.110000,-1.536000,8.562500>}
box{<0,0,-0.076200><4.700000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.410000,-1.536000,8.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.540000,-1.536000,8.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.410000,-1.536000,8.562500>}
box{<0,0,-0.076200><0.130000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.410000,-1.536000,8.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.160000,-1.536000,8.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.800000,-1.536000,8.562500>}
box{<0,0,-0.076200><0.640000,0.036000,0.076200> rotate<0,0.000000,0> translate<19.160000,-1.536000,8.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.430000,-1.536000,8.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.070000,-1.536000,8.562500>}
box{<0,0,-0.076200><0.640000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.430000,-1.536000,8.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.700000,-1.536000,8.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<22.340000,-1.536000,8.562500>}
box{<0,0,-0.076200><0.640000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.700000,-1.536000,8.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<22.960000,-1.536000,8.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.110000,-1.536000,8.562500>}
box{<0,0,-0.076200><0.150000,0.036000,0.076200> rotate<0,0.000000,0> translate<22.960000,-1.536000,8.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<22.950000,-1.536000,13.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.080000,-1.536000,13.562500>}
box{<0,0,-0.076200><0.130000,0.036000,0.076200> rotate<0,0.000000,0> translate<22.950000,-1.536000,13.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<22.340000,-1.536000,13.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.690000,-1.536000,13.562500>}
box{<0,0,-0.076200><0.650000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.690000,-1.536000,13.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.070000,-1.536000,13.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.420000,-1.536000,13.562500>}
box{<0,0,-0.076200><0.650000,0.036000,0.076200> rotate<0,0.000000,0> translate<20.420000,-1.536000,13.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.800000,-1.536000,13.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.160000,-1.536000,13.562500>}
box{<0,0,-0.076200><0.640000,0.036000,0.076200> rotate<0,0.000000,0> translate<19.160000,-1.536000,13.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.540000,-1.536000,13.562500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.410000,-1.536000,13.562500>}
box{<0,0,-0.076200><0.130000,0.036000,0.076200> rotate<0,0.000000,0> translate<18.410000,-1.536000,13.562500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.150000,-1.536000,8.812500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.150000,-1.536000,13.302500>}
box{<0,0,-0.076200><4.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<18.150000,-1.536000,13.302500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.350000,-1.536000,13.312500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.350000,-1.536000,8.812500>}
box{<0,0,-0.076200><4.500000,0.036000,0.076200> rotate<0,-90.000000,0> translate<23.350000,-1.536000,8.812500> }
difference{
cylinder{<19.330000,0,12.177500><19.330000,0.036000,12.177500>0.525400 translate<0,-1.536000,0>}
cylinder{<19.330000,-0.1,12.177500><19.330000,0.135000,12.177500>0.474600 translate<0,-1.536000,0>}}
box{<-0.175000,0,-0.450000><0.175000,0.036000,0.450000> rotate<0,-0.000000,0> translate<18.845000,-1.536000,8.112500>}
box{<-0.175000,0,-0.450000><0.175000,0.036000,0.450000> rotate<0,-0.000000,0> translate<20.115000,-1.536000,8.112500>}
box{<-0.175000,0,-0.450000><0.175000,0.036000,0.450000> rotate<0,-0.000000,0> translate<21.385000,-1.536000,8.112500>}
box{<-0.175000,0,-0.450000><0.175000,0.036000,0.450000> rotate<0,-0.000000,0> translate<22.655000,-1.536000,8.112500>}
box{<-0.175000,0,-0.450000><0.175000,0.036000,0.450000> rotate<0,-0.000000,0> translate<22.645000,-1.536000,14.012500>}
box{<-0.175000,0,-0.450000><0.175000,0.036000,0.450000> rotate<0,-0.000000,0> translate<21.375000,-1.536000,14.012500>}
box{<-0.175000,0,-0.450000><0.175000,0.036000,0.450000> rotate<0,-0.000000,0> translate<20.105000,-1.536000,14.012500>}
box{<-0.175000,0,-0.450000><0.175000,0.036000,0.450000> rotate<0,-0.000000,0> translate<18.845000,-1.536000,14.012500>}
//IC5 silk screen
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<53.119163,0.000000,22.030328>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<53.119163,0.000000,20.969669>}
box{<0,0,-0.127000><1.060659,0.036000,0.127000> rotate<0,-90.000000,0> translate<53.119163,0.000000,20.969669> }
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<53.119163,0.000000,20.969669>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<43.926775,0.000000,11.777281>}
box{<0,0,-0.127000><12.999999,0.036000,0.127000> rotate<0,-44.997030,0> translate<43.926775,0.000000,11.777281> }
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<43.926775,0.000000,11.777281>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<43.573222,0.000000,11.777281>}
box{<0,0,-0.127000><0.353553,0.036000,0.127000> rotate<0,0.000000,0> translate<43.573222,0.000000,11.777281> }
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<43.573222,0.000000,11.777281>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<34.027281,0.000000,21.323222>}
box{<0,0,-0.127000><13.499999,0.036000,0.127000> rotate<0,44.997030,0> translate<34.027281,0.000000,21.323222> }
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<34.027281,0.000000,21.323222>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<34.027281,0.000000,21.676775>}
box{<0,0,-0.127000><0.353553,0.036000,0.127000> rotate<0,90.000000,0> translate<34.027281,0.000000,21.676775> }
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<34.027281,0.000000,21.676775>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<43.573222,0.000000,31.222716>}
box{<0,0,-0.127000><13.499999,0.036000,0.127000> rotate<0,-44.997030,0> translate<34.027281,0.000000,21.676775> }
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<43.573222,0.000000,31.222716>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<43.926775,0.000000,31.222716>}
box{<0,0,-0.127000><0.353553,0.036000,0.127000> rotate<0,0.000000,0> translate<43.573222,0.000000,31.222716> }
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<43.926775,0.000000,31.222716>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<53.119163,0.000000,22.030328>}
box{<0,0,-0.127000><12.999999,0.036000,0.127000> rotate<0,44.997030,0> translate<43.926775,0.000000,31.222716> }
difference{
cylinder{<52.235281,0,21.499997><52.235281,0.036000,21.499997>0.376900 translate<0,0.000000,0>}
cylinder{<52.235281,-0.1,21.499997><52.235281,0.135000,21.499997>0.122900 translate<0,0.000000,0>}}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<53.419684,0.000000,22.684403>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<53.066131,0.000000,23.037956>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<52.712578,0.000000,23.391509>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<52.359025,0.000000,23.745062>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<52.005469,0.000000,24.098616>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<51.651916,0.000000,24.452169>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<51.298363,0.000000,24.805722>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<50.944809,0.000000,25.159275>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<50.591256,0.000000,25.512828>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<50.237703,0.000000,25.866381>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<49.884150,0.000000,26.219938>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<49.530597,0.000000,26.573491>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<49.177044,0.000000,26.927044>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<48.823491,0.000000,27.280597>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<48.469938,0.000000,27.634150>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<48.116381,0.000000,27.987703>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<47.762828,0.000000,28.341256>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<47.409275,0.000000,28.694809>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<47.055722,0.000000,29.048362>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<46.702169,0.000000,29.401916>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<46.348616,0.000000,29.755469>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<45.995063,0.000000,30.109025>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<45.641509,0.000000,30.462578>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<45.287956,0.000000,30.816131>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<44.934403,0.000000,31.169684>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<42.565594,0.000000,31.169684>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<42.212041,0.000000,30.816131>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<41.858488,0.000000,30.462578>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<41.504934,0.000000,30.109025>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<41.151381,0.000000,29.755469>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<40.797828,0.000000,29.401916>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<40.444275,0.000000,29.048362>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<40.090722,0.000000,28.694809>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<39.737169,0.000000,28.341256>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<39.383616,0.000000,27.987703>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<39.030059,0.000000,27.634150>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<38.676506,0.000000,27.280597>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<38.322953,0.000000,26.927044>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<37.969400,0.000000,26.573491>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<37.615847,0.000000,26.219938>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<37.262294,0.000000,25.866381>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<36.908741,0.000000,25.512828>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<36.555188,0.000000,25.159275>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<36.201634,0.000000,24.805722>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<35.848081,0.000000,24.452169>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<35.494528,0.000000,24.098616>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<35.140972,0.000000,23.745062>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<34.787419,0.000000,23.391509>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<34.433866,0.000000,23.037956>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<34.080313,0.000000,22.684403>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<34.080313,0.000000,20.315594>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<34.433866,0.000000,19.962041>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<34.787419,0.000000,19.608488>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<35.140972,0.000000,19.254934>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<35.494528,0.000000,18.901381>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<35.848081,0.000000,18.547828>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<36.201634,0.000000,18.194275>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<36.555188,0.000000,17.840722>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<36.908741,0.000000,17.487169>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<37.262294,0.000000,17.133616>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<37.615847,0.000000,16.780059>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<37.969400,0.000000,16.426506>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<38.322953,0.000000,16.072953>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<38.676506,0.000000,15.719400>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<39.030059,0.000000,15.365847>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<39.383616,0.000000,15.012294>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<39.737169,0.000000,14.658741>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<40.090722,0.000000,14.305188>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<40.444275,0.000000,13.951634>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<40.797828,0.000000,13.598081>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<41.151381,0.000000,13.244528>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<41.504934,0.000000,12.890972>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<41.858488,0.000000,12.537419>}
box{<-0.524900,0,-0.150000><0.524900,0.036000,0.150000> rotate<0,-225.000000,0> translate<42.212041,0.000000,12.183866>}
box{<-0.524900,0,-0.150100><0.524900,0.036000,0.150100> rotate<0,-225.000000,0> translate<42.565594,0.000000,11.830312>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<44.934403,0.000000,11.830312>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<45.287956,0.000000,12.183866>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<45.641509,0.000000,12.537419>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<45.995063,0.000000,12.890972>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<46.348616,0.000000,13.244528>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<46.702169,0.000000,13.598081>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<47.055722,0.000000,13.951634>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<47.409275,0.000000,14.305188>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<47.762828,0.000000,14.658741>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<48.116381,0.000000,15.012294>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<48.469938,0.000000,15.365847>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<48.823491,0.000000,15.719400>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<49.177044,0.000000,16.072953>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<49.530597,0.000000,16.426506>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<49.884150,0.000000,16.780059>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<50.237703,0.000000,17.133616>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<50.591256,0.000000,17.487169>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<50.944809,0.000000,17.840722>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<51.298363,0.000000,18.194275>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<51.651916,0.000000,18.547828>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<52.005469,0.000000,18.901381>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<52.359025,0.000000,19.254934>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<52.712578,0.000000,19.608488>}
box{<-0.150000,0,-0.524900><0.150000,0.036000,0.524900> rotate<0,-225.000000,0> translate<53.066131,0.000000,19.962041>}
box{<-0.150100,0,-0.524900><0.150100,0.036000,0.524900> rotate<0,-225.000000,0> translate<53.419684,0.000000,20.315594>}
//IC6 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.245000,0.000000,7.557500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.245000,0.000000,14.567500>}
box{<0,0,-0.076200><7.010000,0.036000,0.076200> rotate<0,90.000000,0> translate<17.245000,0.000000,14.567500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.245000,0.000000,14.567500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<24.255000,0.000000,14.567500>}
box{<0,0,-0.076200><7.010000,0.036000,0.076200> rotate<0,0.000000,0> translate<17.245000,0.000000,14.567500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<24.255000,0.000000,14.567500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<24.255000,0.000000,7.912500>}
box{<0,0,-0.076200><6.655000,0.036000,0.076200> rotate<0,-90.000000,0> translate<24.255000,0.000000,7.912500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.900000,0.000000,7.557500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.245000,0.000000,7.557500>}
box{<0,0,-0.076200><6.655000,0.036000,0.076200> rotate<0,0.000000,0> translate<17.245000,0.000000,7.557500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.900000,0.000000,7.557500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<24.255000,0.000000,7.912500>}
box{<0,0,-0.076200><0.502046,0.036000,0.076200> rotate<0,-44.997030,0> translate<23.900000,0.000000,7.557500> }
difference{
cylinder{<23.493200,0,8.319300><23.493200,0.036000,8.319300>0.435400 translate<0,0.000000,0>}
cylinder{<23.493200,-0.1,8.319300><23.493200,0.135000,8.319300>0.283000 translate<0,0.000000,0>}}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<24.801300,0.000000,8.262500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<24.801300,0.000000,9.062500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<24.801300,0.000000,9.862500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<24.801300,0.000000,10.662500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<24.801300,0.000000,11.462500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<24.801300,0.000000,12.262500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<24.801300,0.000000,13.062500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<24.801300,0.000000,13.862500>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<23.550000,0.000000,15.113800>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<22.750000,0.000000,15.113800>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<21.950000,0.000000,15.113800>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<21.150000,0.000000,15.113800>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<20.350000,0.000000,15.113800>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<19.550000,0.000000,15.113800>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<18.750000,0.000000,15.113800>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<17.950000,0.000000,15.113800>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<16.698700,0.000000,13.862500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<16.698700,0.000000,13.062500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<16.698700,0.000000,12.262500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<16.698700,0.000000,11.462500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<16.698700,0.000000,10.662500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<16.698700,0.000000,9.862500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<16.698700,0.000000,9.062500>}
box{<-0.495300,0,-0.228600><0.495300,0.036000,0.228600> rotate<0,-180.000000,0> translate<16.698700,0.000000,8.262500>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<17.950000,0.000000,7.011200>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<18.750000,0.000000,7.011200>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<19.550000,0.000000,7.011200>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<20.350000,0.000000,7.011200>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<21.150000,0.000000,7.011200>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<21.950000,0.000000,7.011200>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<22.750000,0.000000,7.011200>}
box{<-0.228600,0,-0.495300><0.228600,0.036000,0.495300> rotate<0,-180.000000,0> translate<23.550000,0.000000,7.011200>}
//IC7 silk screen
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.725000,-1.536000,4.337500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<4.525000,-1.536000,4.337500>}
box{<0,0,-0.101600><10.200000,0.036000,0.101600> rotate<0,0.000000,0> translate<4.525000,-1.536000,4.337500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<4.525000,-1.536000,4.337500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<4.525000,-1.536000,9.537500>}
box{<0,0,-0.101600><5.200000,0.036000,0.101600> rotate<0,90.000000,0> translate<4.525000,-1.536000,9.537500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<4.525000,-1.536000,9.537500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.725000,-1.536000,9.537500>}
box{<0,0,-0.101600><10.200000,0.036000,0.101600> rotate<0,0.000000,0> translate<4.525000,-1.536000,9.537500> }
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.725000,-1.536000,9.537500>}
cylinder{<0,0,0><0,0.036000,0>0.101600 translate<14.725000,-1.536000,4.337500>}
box{<0,0,-0.101600><5.200000,0.036000,0.101600> rotate<0,-90.000000,0> translate<14.725000,-1.536000,4.337500> }
difference{
cylinder{<13.825000,0,5.312500><13.825000,0.036000,5.312500>0.442200 translate<0,-1.536000,0>}
cylinder{<13.825000,-0.1,5.312500><13.825000,0.135000,5.312500>0.442200 translate<0,-1.536000,0>}}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<13.850000,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<13.200100,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<12.550100,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<11.900100,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<11.250100,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<10.600100,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<9.950100,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<9.299900,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<8.649900,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<7.999900,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<7.349900,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<6.699900,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<6.049900,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<5.400000,-1.536000,3.648200>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<5.400000,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<6.049900,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<6.699900,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<7.349900,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<7.999900,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<8.649900,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<9.299900,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<9.950100,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<10.600100,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<11.250100,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<11.900100,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<12.550100,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<13.200100,-1.536000,10.226800>}
box{<-0.177800,0,-0.647700><0.177800,0.036000,0.647700> rotate<0,-180.000000,0> translate<13.850000,-1.536000,10.226800>}
//IC8 silk screen
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<16.637500,-1.536000,33.575000>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<16.637500,-1.536000,48.225000>}
box{<0,0,-0.127000><14.650000,0.036000,0.127000> rotate<0,90.000000,0> translate<16.637500,-1.536000,48.225000> }
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<22.937500,-1.536000,33.575000>}
cylinder{<0,0,0><0,0.036000,0>0.127000 translate<22.937500,-1.536000,48.175000>}
box{<0,0,-0.127000><14.600000,0.036000,0.127000> rotate<0,90.000000,0> translate<22.937500,-1.536000,48.175000> }
cylinder{<0,0,0><0,0.036000,0>0.125000 translate<22.937500,-1.536000,48.225000>}
cylinder{<0,0,0><0,0.036000,0>0.125000 translate<16.637500,-1.536000,48.225000>}
box{<0,0,-0.125000><6.300000,0.036000,0.125000> rotate<0,0.000000,0> translate<16.637500,-1.536000,48.225000> }
cylinder{<0,0,0><0,0.036000,0>0.125000 translate<16.637500,-1.536000,33.525000>}
cylinder{<0,0,0><0,0.036000,0>0.125000 translate<22.937500,-1.536000,33.525000>}
box{<0,0,-0.125000><6.300000,0.036000,0.125000> rotate<0,0.000000,0> translate<16.637500,-1.536000,33.525000> }
difference{
cylinder{<22.237500,0,47.525000><22.237500,0.036000,47.525000>0.238800 translate<0,-1.536000,0>}
cylinder{<22.237500,-0.1,47.525000><22.237500,0.135000,47.525000>0.000000 translate<0,-1.536000,0>}}
//JP1 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.000000,0.000000,19.421500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.746000,0.000000,19.167500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<57.746000,0.000000,19.167500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.000000,0.000000,19.421500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.254000,0.000000,19.167500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<58.000000,0.000000,19.421500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.000000,0.000000,21.453500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.746000,0.000000,21.707500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<57.746000,0.000000,21.707500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.000000,0.000000,21.453500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.254000,0.000000,21.707500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<58.000000,0.000000,21.453500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.254000,0.000000,21.707500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.286000,0.000000,21.707500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<58.254000,0.000000,21.707500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.540000,0.000000,21.453500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.286000,0.000000,21.707500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<60.286000,0.000000,21.707500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.714000,0.000000,21.707500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.460000,0.000000,21.453500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<55.460000,0.000000,21.453500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.714000,0.000000,21.707500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.746000,0.000000,21.707500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<55.714000,0.000000,21.707500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.460000,0.000000,21.453500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.460000,0.000000,19.421500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<55.460000,0.000000,19.421500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.714000,0.000000,19.167500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.460000,0.000000,19.421500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<55.460000,0.000000,19.421500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.714000,0.000000,19.167500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.746000,0.000000,19.167500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<55.714000,0.000000,19.167500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.254000,0.000000,19.167500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.286000,0.000000,19.167500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<58.254000,0.000000,19.167500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.540000,0.000000,19.421500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.286000,0.000000,19.167500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<60.286000,0.000000,19.167500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.540000,0.000000,19.421500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.540000,0.000000,21.453500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<60.540000,0.000000,21.453500> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<56.730000,0.000000,20.437500>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<59.270000,0.000000,20.437500>}
//JP2 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.000000,0.000000,24.796500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.746000,0.000000,24.542500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<57.746000,0.000000,24.542500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.000000,0.000000,24.796500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.254000,0.000000,24.542500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<58.000000,0.000000,24.796500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.000000,0.000000,26.828500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.746000,0.000000,27.082500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<57.746000,0.000000,27.082500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.000000,0.000000,26.828500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.254000,0.000000,27.082500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<58.000000,0.000000,26.828500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.254000,0.000000,27.082500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.286000,0.000000,27.082500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<58.254000,0.000000,27.082500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.540000,0.000000,26.828500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.286000,0.000000,27.082500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<60.286000,0.000000,27.082500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.714000,0.000000,27.082500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.460000,0.000000,26.828500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<55.460000,0.000000,26.828500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.714000,0.000000,27.082500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.746000,0.000000,27.082500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<55.714000,0.000000,27.082500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.460000,0.000000,26.828500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.460000,0.000000,24.796500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<55.460000,0.000000,24.796500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.714000,0.000000,24.542500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.460000,0.000000,24.796500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<55.460000,0.000000,24.796500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<55.714000,0.000000,24.542500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.746000,0.000000,24.542500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<55.714000,0.000000,24.542500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.254000,0.000000,24.542500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.286000,0.000000,24.542500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<58.254000,0.000000,24.542500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.540000,0.000000,24.796500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.286000,0.000000,24.542500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<60.286000,0.000000,24.542500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.540000,0.000000,24.796500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.540000,0.000000,26.828500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<60.540000,0.000000,26.828500> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<56.730000,0.000000,25.812500>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<59.270000,0.000000,25.812500>}
//JP3 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.359000,0.000000,22.750000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.105000,0.000000,23.004000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<30.105000,0.000000,23.004000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.359000,0.000000,22.750000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.105000,0.000000,22.496000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<30.105000,0.000000,22.496000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.391000,0.000000,22.750000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.645000,0.000000,23.004000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<32.391000,0.000000,22.750000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.391000,0.000000,22.750000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.645000,0.000000,22.496000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<32.391000,0.000000,22.750000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.645000,0.000000,22.496000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.645000,0.000000,20.464000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<32.645000,0.000000,20.464000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.391000,0.000000,20.210000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.645000,0.000000,20.464000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<32.391000,0.000000,20.210000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.645000,0.000000,25.036000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.391000,0.000000,25.290000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<32.391000,0.000000,25.290000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.645000,0.000000,25.036000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.645000,0.000000,23.004000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<32.645000,0.000000,23.004000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.391000,0.000000,25.290000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.359000,0.000000,25.290000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<30.359000,0.000000,25.290000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.105000,0.000000,25.036000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.359000,0.000000,25.290000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<30.105000,0.000000,25.036000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.105000,0.000000,25.036000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.105000,0.000000,23.004000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<30.105000,0.000000,23.004000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.105000,0.000000,22.496000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.105000,0.000000,20.464000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<30.105000,0.000000,20.464000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.359000,0.000000,20.210000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.105000,0.000000,20.464000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<30.105000,0.000000,20.464000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.359000,0.000000,20.210000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.391000,0.000000,20.210000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<30.359000,0.000000,20.210000> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-0.000000,0> translate<31.375000,0.000000,24.020000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-0.000000,0> translate<31.375000,0.000000,21.480000>}
//JP4 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.355000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.609000,0.000000,23.020000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<23.355000,0.000000,22.766000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.355000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.101000,0.000000,23.020000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<23.101000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.355000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.609000,0.000000,20.480000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<23.355000,0.000000,20.734000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.355000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.101000,0.000000,20.480000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<23.101000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.101000,0.000000,20.480000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.069000,0.000000,20.480000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.069000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.815000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.069000,0.000000,20.480000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<20.815000,0.000000,20.734000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.641000,0.000000,20.480000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.609000,0.000000,20.480000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<23.609000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.641000,0.000000,23.020000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.609000,0.000000,23.020000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<23.609000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.101000,0.000000,23.020000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.069000,0.000000,23.020000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.069000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.815000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.069000,0.000000,23.020000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<20.815000,0.000000,22.766000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.815000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.815000,0.000000,20.734000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<20.815000,0.000000,20.734000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.895000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.149000,0.000000,23.020000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<25.895000,0.000000,22.766000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.895000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.641000,0.000000,23.020000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<25.641000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.895000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.149000,0.000000,20.480000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<25.895000,0.000000,20.734000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.895000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.641000,0.000000,20.480000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<25.641000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.181000,0.000000,20.480000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.435000,0.000000,20.734000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<28.181000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.181000,0.000000,20.480000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.149000,0.000000,20.480000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<26.149000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.435000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.435000,0.000000,22.766000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<28.435000,0.000000,22.766000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.181000,0.000000,23.020000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.435000,0.000000,22.766000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<28.181000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.181000,0.000000,23.020000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.149000,0.000000,23.020000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<26.149000,0.000000,23.020000> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-270.000000,0> translate<24.625000,0.000000,21.750000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-270.000000,0> translate<22.085000,0.000000,21.750000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-270.000000,0> translate<27.165000,0.000000,21.750000>}
//JP5 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.375000,0.000000,2.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.121000,0.000000,2.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<21.121000,0.000000,2.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.375000,0.000000,2.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.629000,0.000000,2.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<21.375000,0.000000,2.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.375000,0.000000,4.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.121000,0.000000,4.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<21.121000,0.000000,4.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.375000,0.000000,4.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.629000,0.000000,4.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<21.375000,0.000000,4.391000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.629000,0.000000,4.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.661000,0.000000,4.645000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.629000,0.000000,4.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.915000,0.000000,4.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.661000,0.000000,4.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<23.661000,0.000000,4.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.089000,0.000000,4.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.835000,0.000000,4.391000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<18.835000,0.000000,4.391000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.089000,0.000000,4.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.121000,0.000000,4.645000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<19.089000,0.000000,4.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.835000,0.000000,4.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.835000,0.000000,2.359000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<18.835000,0.000000,2.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.089000,0.000000,2.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<18.835000,0.000000,2.359000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<18.835000,0.000000,2.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.089000,0.000000,2.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.121000,0.000000,2.105000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<19.089000,0.000000,2.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.629000,0.000000,2.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.661000,0.000000,2.105000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.629000,0.000000,2.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.915000,0.000000,2.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.661000,0.000000,2.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<23.661000,0.000000,2.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.915000,0.000000,2.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.915000,0.000000,4.391000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<23.915000,0.000000,4.391000> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<20.105000,0.000000,3.375000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<22.645000,0.000000,3.375000>}
//JP6 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.895000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.641000,0.000000,16.730000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<25.641000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.895000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.149000,0.000000,16.730000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<25.895000,0.000000,16.984000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.895000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.641000,0.000000,19.270000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<25.641000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.895000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.149000,0.000000,19.270000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<25.895000,0.000000,19.016000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.149000,0.000000,19.270000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.181000,0.000000,19.270000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<26.149000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.435000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.181000,0.000000,19.270000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<28.181000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.609000,0.000000,19.270000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.641000,0.000000,19.270000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<23.609000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.609000,0.000000,16.730000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.641000,0.000000,16.730000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<23.609000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<26.149000,0.000000,16.730000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.181000,0.000000,16.730000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<26.149000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.435000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.181000,0.000000,16.730000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<28.181000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.435000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.435000,0.000000,19.016000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<28.435000,0.000000,19.016000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.355000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.101000,0.000000,16.730000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<23.101000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.355000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.609000,0.000000,16.730000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<23.355000,0.000000,16.984000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.355000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.101000,0.000000,19.270000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<23.101000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.355000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.609000,0.000000,19.270000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<23.355000,0.000000,19.016000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.069000,0.000000,19.270000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.815000,0.000000,19.016000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<20.815000,0.000000,19.016000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.069000,0.000000,19.270000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.101000,0.000000,19.270000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.069000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.815000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.815000,0.000000,16.984000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<20.815000,0.000000,16.984000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.069000,0.000000,16.730000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.815000,0.000000,16.984000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<20.815000,0.000000,16.984000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.069000,0.000000,16.730000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<23.101000,0.000000,16.730000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.069000,0.000000,16.730000> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<24.625000,0.000000,18.000000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<27.165000,0.000000,18.000000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<22.085000,0.000000,18.000000>}
//JP7 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.500000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.246000,0.000000,16.730000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<17.246000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.500000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.754000,0.000000,16.730000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<17.500000,0.000000,16.984000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.500000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.246000,0.000000,19.270000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<17.246000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.500000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.754000,0.000000,19.270000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<17.500000,0.000000,19.016000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.754000,0.000000,19.270000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.786000,0.000000,19.270000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<17.754000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.040000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.786000,0.000000,19.270000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<19.786000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.214000,0.000000,19.270000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.960000,0.000000,19.016000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<14.960000,0.000000,19.016000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.214000,0.000000,19.270000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.246000,0.000000,19.270000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<15.214000,0.000000,19.270000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.960000,0.000000,19.016000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.960000,0.000000,16.984000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<14.960000,0.000000,16.984000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.214000,0.000000,16.730000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.960000,0.000000,16.984000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<14.960000,0.000000,16.984000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.214000,0.000000,16.730000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.246000,0.000000,16.730000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<15.214000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.754000,0.000000,16.730000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.786000,0.000000,16.730000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<17.754000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.040000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.786000,0.000000,16.730000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<19.786000,0.000000,16.730000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.040000,0.000000,16.984000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.040000,0.000000,19.016000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<20.040000,0.000000,19.016000> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<16.230000,0.000000,18.000000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<18.770000,0.000000,18.000000>}
//JP8 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.500000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.246000,0.000000,20.480000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<17.246000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.500000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.754000,0.000000,20.480000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<17.500000,0.000000,20.734000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.500000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.246000,0.000000,23.020000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<17.246000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.500000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.754000,0.000000,23.020000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<17.500000,0.000000,22.766000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.754000,0.000000,23.020000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.786000,0.000000,23.020000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<17.754000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.040000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.786000,0.000000,23.020000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<19.786000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.214000,0.000000,23.020000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.960000,0.000000,22.766000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<14.960000,0.000000,22.766000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.214000,0.000000,23.020000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.246000,0.000000,23.020000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<15.214000,0.000000,23.020000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.960000,0.000000,22.766000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.960000,0.000000,20.734000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<14.960000,0.000000,20.734000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.214000,0.000000,20.480000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.960000,0.000000,20.734000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<14.960000,0.000000,20.734000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.214000,0.000000,20.480000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.246000,0.000000,20.480000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<15.214000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<17.754000,0.000000,20.480000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.786000,0.000000,20.480000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<17.754000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.040000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.786000,0.000000,20.480000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<19.786000,0.000000,20.480000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.040000,0.000000,20.734000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<20.040000,0.000000,22.766000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<20.040000,0.000000,22.766000> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<16.230000,0.000000,21.750000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<18.770000,0.000000,21.750000>}
//JP9 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.437500,0.000000,1.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.183500,0.000000,1.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<33.183500,0.000000,1.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.437500,0.000000,1.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.691500,0.000000,1.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<33.437500,0.000000,1.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.437500,0.000000,3.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.183500,0.000000,3.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<33.183500,0.000000,3.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.437500,0.000000,3.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.691500,0.000000,3.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<33.437500,0.000000,3.391000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.691500,0.000000,3.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.723500,0.000000,3.645000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<33.691500,0.000000,3.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.977500,0.000000,3.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.723500,0.000000,3.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<35.723500,0.000000,3.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.151500,0.000000,3.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.897500,0.000000,3.391000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<30.897500,0.000000,3.391000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.151500,0.000000,3.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.183500,0.000000,3.645000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.151500,0.000000,3.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.897500,0.000000,3.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.897500,0.000000,1.359000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<30.897500,0.000000,1.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.151500,0.000000,1.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.897500,0.000000,1.359000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<30.897500,0.000000,1.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.151500,0.000000,1.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.183500,0.000000,1.105000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.151500,0.000000,1.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.691500,0.000000,1.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.723500,0.000000,1.105000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<33.691500,0.000000,1.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.977500,0.000000,1.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.723500,0.000000,1.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<35.723500,0.000000,1.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.977500,0.000000,1.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.977500,0.000000,3.391000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<35.977500,0.000000,3.391000> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<32.167500,0.000000,2.375000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<34.707500,0.000000,2.375000>}
//JP10 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.062500,0.000000,1.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<38.808500,0.000000,1.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<38.808500,0.000000,1.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.062500,0.000000,1.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.316500,0.000000,1.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<39.062500,0.000000,1.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.062500,0.000000,3.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<38.808500,0.000000,3.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<38.808500,0.000000,3.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.062500,0.000000,3.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.316500,0.000000,3.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<39.062500,0.000000,3.391000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.316500,0.000000,3.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.348500,0.000000,3.645000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<39.316500,0.000000,3.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.602500,0.000000,3.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.348500,0.000000,3.645000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<41.348500,0.000000,3.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.776500,0.000000,3.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.522500,0.000000,3.391000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<36.522500,0.000000,3.391000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.776500,0.000000,3.645000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<38.808500,0.000000,3.645000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<36.776500,0.000000,3.645000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.522500,0.000000,3.391000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.522500,0.000000,1.359000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<36.522500,0.000000,1.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.776500,0.000000,1.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.522500,0.000000,1.359000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<36.522500,0.000000,1.359000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.776500,0.000000,1.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<38.808500,0.000000,1.105000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<36.776500,0.000000,1.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.316500,0.000000,1.105000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.348500,0.000000,1.105000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<39.316500,0.000000,1.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.602500,0.000000,1.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.348500,0.000000,1.105000>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<41.348500,0.000000,1.105000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.602500,0.000000,1.359000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<41.602500,0.000000,3.391000>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<41.602500,0.000000,3.391000> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<37.792500,0.000000,2.375000>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<40.332500,0.000000,2.375000>}
//JP11 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.750000,0.000000,1.296500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.496000,0.000000,1.042500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<27.496000,0.000000,1.042500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.750000,0.000000,1.296500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.004000,0.000000,1.042500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<27.750000,0.000000,1.296500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.750000,0.000000,3.328500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.496000,0.000000,3.582500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<27.496000,0.000000,3.582500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.750000,0.000000,3.328500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.004000,0.000000,3.582500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<27.750000,0.000000,3.328500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.004000,0.000000,3.582500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.036000,0.000000,3.582500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<28.004000,0.000000,3.582500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.290000,0.000000,3.328500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.036000,0.000000,3.582500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<30.036000,0.000000,3.582500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.464000,0.000000,3.582500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.210000,0.000000,3.328500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<25.210000,0.000000,3.328500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.464000,0.000000,3.582500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.496000,0.000000,3.582500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<25.464000,0.000000,3.582500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.210000,0.000000,3.328500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.210000,0.000000,1.296500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,-90.000000,0> translate<25.210000,0.000000,1.296500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.464000,0.000000,1.042500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.210000,0.000000,1.296500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,44.997030,0> translate<25.210000,0.000000,1.296500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.464000,0.000000,1.042500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.496000,0.000000,1.042500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<25.464000,0.000000,1.042500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<28.004000,0.000000,1.042500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.036000,0.000000,1.042500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,0.000000,0> translate<28.004000,0.000000,1.042500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.290000,0.000000,1.296500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.036000,0.000000,1.042500>}
box{<0,0,-0.076200><0.359210,0.036000,0.076200> rotate<0,-44.997030,0> translate<30.036000,0.000000,1.042500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.290000,0.000000,1.296500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.290000,0.000000,3.328500>}
box{<0,0,-0.076200><2.032000,0.036000,0.076200> rotate<0,90.000000,0> translate<30.290000,0.000000,3.328500> }
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<26.480000,0.000000,2.312500>}
box{<-0.304800,0,-0.304800><0.304800,0.036000,0.304800> rotate<0,-90.000000,0> translate<29.020000,0.000000,2.312500>}
//LED1 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.442500,0.000000,19.849000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.932500,0.000000,19.849000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<65.442500,0.000000,19.849000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.932500,0.000000,19.401000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.442500,0.000000,19.401000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<65.442500,0.000000,19.401000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<65.283500,0.000000,19.620150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<66.096300,0.000000,19.620150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<66.096300,0.000000,19.620150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<66.096300,0.000000,19.620150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<65.842300,0.000000,19.620150>}
//LED2 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.442500,0.000000,21.411500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.932500,0.000000,21.411500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<65.442500,0.000000,21.411500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.932500,0.000000,20.963500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.442500,0.000000,20.963500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<65.442500,0.000000,20.963500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<65.283500,0.000000,21.182650>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<66.096300,0.000000,21.182650>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<66.096300,0.000000,21.182650>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<66.096300,0.000000,21.182650>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<65.842300,0.000000,21.182650>}
//Q1 silk screen
object{ARC(2.032100,0.050800,0.002059,55.773052,0.036000) translate<31.687500,0.000000,15.803928>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.830500,0.000000,18.090000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.846500,0.000000,18.090000>}
box{<0,0,-0.076200><1.016000,0.036000,0.076200> rotate<0,0.000000,0> translate<32.830500,0.000000,18.090000> }
object{ARC(2.032000,0.050800,304.228866,360.001351,0.036000) translate<31.687500,0.000000,8.945953>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.830500,0.000000,6.660000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<33.846500,0.000000,6.660000>}
box{<0,0,-0.076200><1.016000,0.036000,0.076200> rotate<0,0.000000,0> translate<32.830500,0.000000,6.660000> }
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<30.417500,0.000000,8.946000>}
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<30.417500,0.000000,15.804000>}
box{<0,0,-0.025400><6.858000,0.036000,0.025400> rotate<0,90.000000,0> translate<30.417500,0.000000,15.804000> }
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<29.655500,0.000000,8.946000>}
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<29.655500,0.000000,15.804000>}
box{<0,0,-0.025400><6.858000,0.036000,0.025400> rotate<0,90.000000,0> translate<29.655500,0.000000,15.804000> }
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<32.957500,0.000000,15.804000>}
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<32.957500,0.000000,8.946000>}
box{<0,0,-0.025400><6.858000,0.036000,0.025400> rotate<0,-90.000000,0> translate<32.957500,0.000000,8.946000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<29.274500,0.000000,6.914000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<29.274500,0.000000,17.836000>}
box{<0,0,-0.076200><10.922000,0.036000,0.076200> rotate<0,90.000000,0> translate<29.274500,0.000000,17.836000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.306500,0.000000,6.660000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.306500,0.000000,5.898000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,-90.000000,0> translate<31.306500,0.000000,5.898000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.068500,0.000000,6.660000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.068500,0.000000,5.898000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,-90.000000,0> translate<32.068500,0.000000,5.898000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.306500,0.000000,5.898000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.068500,0.000000,5.898000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.306500,0.000000,5.898000> }
object{ARC(0.254000,0.152400,180.000000,270.000000,0.036000) translate<29.528500,0.000000,6.914000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.544500,0.000000,6.660000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.830500,0.000000,6.660000>}
box{<0,0,-0.076200><2.286000,0.036000,0.076200> rotate<0,0.000000,0> translate<30.544500,0.000000,6.660000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<29.528500,0.000000,6.660000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.544500,0.000000,6.660000>}
box{<0,0,-0.076200><1.016000,0.036000,0.076200> rotate<0,0.000000,0> translate<29.528500,0.000000,6.660000> }
object{ARC(1.270000,0.050800,179.999105,205.841933,0.036000) translate<31.687500,0.000000,8.945981>}
object{ARC(1.270000,0.050800,334.158067,360.000895,0.036000) translate<31.687500,0.000000,8.945981>}
object{ARC(2.032000,0.050800,180.003387,235.774544,0.036000) translate<31.687500,0.000000,8.946119>}
object{ARC(1.270009,0.050800,205.842738,334.157262,0.036000) translate<31.687500,0.000000,8.946000>}
object{ARC(2.032041,0.050800,235.771894,304.228106,0.036000) translate<31.687500,0.000000,8.946000>}
object{ARC(2.032000,0.050800,124.228866,180.001351,0.036000) translate<31.687500,0.000000,15.804047>}
object{ARC(1.270009,0.050800,25.842738,154.157262,0.036000) translate<31.687500,0.000000,15.804000>}
object{ARC(1.270000,0.050800,154.158067,180.000895,0.036000) translate<31.687500,0.000000,15.804019>}
object{ARC(1.270000,0.050800,359.999105,385.841933,0.036000) translate<31.687500,0.000000,15.804019>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.306500,0.000000,18.852000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.068500,0.000000,18.852000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.306500,0.000000,18.852000> }
object{ARC(2.032041,0.050800,55.771894,124.228106,0.036000) translate<31.687500,0.000000,15.804000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.544500,0.000000,18.090000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.306500,0.000000,18.090000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,0.000000,0> translate<30.544500,0.000000,18.090000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.306500,0.000000,18.090000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.068500,0.000000,18.090000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.306500,0.000000,18.090000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.068500,0.000000,18.090000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.830500,0.000000,18.090000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,0.000000,0> translate<32.068500,0.000000,18.090000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<29.528500,0.000000,18.090000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<30.544500,0.000000,18.090000>}
box{<0,0,-0.076200><1.016000,0.036000,0.076200> rotate<0,0.000000,0> translate<29.528500,0.000000,18.090000> }
object{ARC(0.254000,0.152400,90.000000,180.000000,0.036000) translate<29.528500,0.000000,17.836000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.306500,0.000000,18.090000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.306500,0.000000,18.852000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,90.000000,0> translate<31.306500,0.000000,18.852000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.068500,0.000000,18.090000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.068500,0.000000,18.852000>}
box{<0,0,-0.076200><0.762000,0.036000,0.076200> rotate<0,90.000000,0> translate<32.068500,0.000000,18.852000> }
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<33.719500,0.000000,15.804000>}
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<33.719500,0.000000,8.946000>}
box{<0,0,-0.025400><6.858000,0.036000,0.025400> rotate<0,-90.000000,0> translate<33.719500,0.000000,8.946000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.100500,0.000000,6.914000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.100500,0.000000,17.836000>}
box{<0,0,-0.076200><10.922000,0.036000,0.076200> rotate<0,90.000000,0> translate<34.100500,0.000000,17.836000> }
object{ARC(0.254000,0.152400,270.000000,360.000000,0.036000) translate<33.846500,0.000000,6.914000>}
object{ARC(0.254000,0.152400,0.000000,90.000000,0.036000) translate<33.846500,0.000000,17.836000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.322500,0.000000,12.629000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.052500,0.000000,12.629000>}
box{<0,0,-0.076200><1.270000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.052500,0.000000,12.629000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.052500,0.000000,12.629000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.052500,0.000000,12.121000>}
box{<0,0,-0.076200><0.508000,0.036000,0.076200> rotate<0,-90.000000,0> translate<31.052500,0.000000,12.121000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.052500,0.000000,12.121000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.322500,0.000000,12.121000>}
box{<0,0,-0.076200><1.270000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.052500,0.000000,12.121000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.322500,0.000000,12.121000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.322500,0.000000,12.629000>}
box{<0,0,-0.076200><0.508000,0.036000,0.076200> rotate<0,90.000000,0> translate<32.322500,0.000000,12.629000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.322500,0.000000,13.010000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.687500,0.000000,13.010000>}
box{<0,0,-0.076200><0.635000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.687500,0.000000,13.010000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.687500,0.000000,13.010000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.052500,0.000000,13.010000>}
box{<0,0,-0.076200><0.635000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.052500,0.000000,13.010000> }
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<31.687500,0.000000,13.010000>}
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<31.687500,0.000000,13.391000>}
box{<0,0,-0.025400><0.381000,0.036000,0.025400> rotate<0,90.000000,0> translate<31.687500,0.000000,13.391000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<32.322500,0.000000,11.740000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.687500,0.000000,11.740000>}
box{<0,0,-0.076200><0.635000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.687500,0.000000,11.740000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.687500,0.000000,11.740000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<31.052500,0.000000,11.740000>}
box{<0,0,-0.076200><0.635000,0.036000,0.076200> rotate<0,0.000000,0> translate<31.052500,0.000000,11.740000> }
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<31.687500,0.000000,11.740000>}
cylinder{<0,0,0><0,0.036000,0>0.025400 translate<31.687500,0.000000,11.359000>}
box{<0,0,-0.025400><0.381000,0.036000,0.025400> rotate<0,-90.000000,0> translate<31.687500,0.000000,11.359000> }
//R1 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<66.245000,0.000000,40.651000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.755000,0.000000,40.651000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<65.755000,0.000000,40.651000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<65.755000,0.000000,41.099000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<66.245000,0.000000,41.099000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<65.755000,0.000000,41.099000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<66.404000,0.000000,40.879850>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<65.591200,0.000000,40.879850>}
//R2 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<31.525000,0.000000,25.912500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<34.725000,0.000000,25.912500>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,0.000000,0> translate<31.525000,0.000000,25.912500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<34.725000,0.000000,25.912500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<34.725000,0.000000,28.712500>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,90.000000,0> translate<34.725000,0.000000,28.712500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<34.725000,0.000000,28.712500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<31.525000,0.000000,28.712500>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,0.000000,0> translate<31.525000,0.000000,28.712500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<31.525000,0.000000,28.712500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<31.525000,0.000000,25.912500>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,-90.000000,0> translate<31.525000,0.000000,25.912500> }
//R3 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.025000,0.000000,35.400000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.025000,0.000000,38.600000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,90.000000,0> translate<37.025000,0.000000,38.600000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.025000,0.000000,38.600000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<34.225000,0.000000,38.600000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<34.225000,0.000000,38.600000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<34.225000,0.000000,38.600000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<34.225000,0.000000,35.400000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,-90.000000,0> translate<34.225000,0.000000,35.400000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<34.225000,0.000000,35.400000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.025000,0.000000,35.400000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<34.225000,0.000000,35.400000> }
//R4 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.474000,0.000000,21.995000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.474000,0.000000,21.505000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<14.474000,0.000000,21.505000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.026000,0.000000,21.505000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.026000,0.000000,21.995000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<14.026000,0.000000,21.995000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<14.245150,0.000000,22.154000>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<14.245150,0.000000,21.341200>}
//R5 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.005000,0.000000,23.911500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.495000,0.000000,23.911500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<57.005000,0.000000,23.911500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.495000,0.000000,23.463500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.005000,0.000000,23.463500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<57.005000,0.000000,23.463500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<56.846000,0.000000,23.682650>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<57.658800,0.000000,23.682650>}
//R6 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.005000,0.000000,18.474000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.495000,0.000000,18.474000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<57.005000,0.000000,18.474000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.495000,0.000000,18.026000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<57.005000,0.000000,18.026000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<57.005000,0.000000,18.026000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<56.846000,0.000000,18.245150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<57.658800,0.000000,18.245150>}
//R7 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<59.776000,0.000000,9.380000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<59.776000,0.000000,9.870000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<59.776000,0.000000,9.870000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.224000,0.000000,9.870000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<60.224000,0.000000,9.380000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<60.224000,0.000000,9.380000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<60.004850,0.000000,9.221000>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<60.004850,0.000000,10.033800>}
//R8 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.358578,0.000000,31.253678>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.621319,0.000000,33.516419>}
box{<0,0,-0.063500><3.199998,0.036000,0.063500> rotate<0,-44.997030,0> translate<35.358578,0.000000,31.253678> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.621319,0.000000,33.516419>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.641419,0.000000,35.496319>}
box{<0,0,-0.063500><2.800001,0.036000,0.063500> rotate<0,44.997030,0> translate<35.641419,0.000000,35.496319> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.641419,0.000000,35.496319>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.378678,0.000000,33.233578>}
box{<0,0,-0.063500><3.199998,0.036000,0.063500> rotate<0,-44.997030,0> translate<33.378678,0.000000,33.233578> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.378678,0.000000,33.233578>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.358578,0.000000,31.253678>}
box{<0,0,-0.063500><2.800001,0.036000,0.063500> rotate<0,44.997030,0> translate<33.378678,0.000000,33.233578> }
//R9 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<36.712500,0.000000,42.025000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<36.712500,0.000000,45.225000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,90.000000,0> translate<36.712500,0.000000,45.225000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<36.712500,0.000000,45.225000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.912500,0.000000,45.225000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<33.912500,0.000000,45.225000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.912500,0.000000,45.225000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.912500,0.000000,42.025000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,-90.000000,0> translate<33.912500,0.000000,42.025000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.912500,0.000000,42.025000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<36.712500,0.000000,42.025000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<33.912500,0.000000,42.025000> }
//R10 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<36.712500,0.000000,38.525000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<36.712500,0.000000,41.725000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,90.000000,0> translate<36.712500,0.000000,41.725000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<36.712500,0.000000,41.725000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.912500,0.000000,41.725000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<33.912500,0.000000,41.725000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.912500,0.000000,41.725000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.912500,0.000000,38.525000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,-90.000000,0> translate<33.912500,0.000000,38.525000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<33.912500,0.000000,38.525000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<36.712500,0.000000,38.525000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<33.912500,0.000000,38.525000> }
//R11 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.837500,0.000000,49.025000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.837500,0.000000,52.225000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,90.000000,0> translate<37.837500,0.000000,52.225000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.837500,0.000000,52.225000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.037500,0.000000,52.225000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<35.037500,0.000000,52.225000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.037500,0.000000,52.225000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.037500,0.000000,49.025000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,-90.000000,0> translate<35.037500,0.000000,49.025000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.037500,0.000000,49.025000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.837500,0.000000,49.025000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<35.037500,0.000000,49.025000> }
//R12 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.837500,0.000000,45.525000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.837500,0.000000,48.725000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,90.000000,0> translate<37.837500,0.000000,48.725000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.837500,0.000000,48.725000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.037500,0.000000,48.725000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<35.037500,0.000000,48.725000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.037500,0.000000,48.725000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.037500,0.000000,45.525000>}
box{<0,0,-0.063500><3.200000,0.036000,0.063500> rotate<0,-90.000000,0> translate<35.037500,0.000000,45.525000> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<35.037500,0.000000,45.525000>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<37.837500,0.000000,45.525000>}
box{<0,0,-0.063500><2.800000,0.036000,0.063500> rotate<0,0.000000,0> translate<35.037500,0.000000,45.525000> }
//R13 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.338500,-1.536000,5.870000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.338500,-1.536000,5.380000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<15.338500,-1.536000,5.380000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.786500,-1.536000,5.380000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<15.786500,-1.536000,5.870000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<15.786500,-1.536000,5.870000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<15.567350,-1.536000,6.029000>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-270.000000,0> translate<15.567350,-1.536000,5.216200>}
//R14 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.255000,0.000000,15.911500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.745000,0.000000,15.911500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<25.255000,0.000000,15.911500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.745000,0.000000,15.463500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<25.255000,0.000000,15.463500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<25.255000,0.000000,15.463500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<25.096000,0.000000,15.682650>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<25.908800,0.000000,15.682650>}
//R15 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.380000,0.000000,25.724000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.870000,0.000000,25.724000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<27.380000,0.000000,25.724000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.870000,0.000000,25.276000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<27.380000,0.000000,25.276000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<27.380000,0.000000,25.276000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<27.221000,0.000000,25.495150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<28.033800,0.000000,25.495150>}
//R16 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<22.120000,0.000000,5.088500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.630000,0.000000,5.088500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.630000,0.000000,5.088500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<21.630000,0.000000,5.536500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<22.120000,0.000000,5.536500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<21.630000,0.000000,5.536500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<22.279000,0.000000,5.317350>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<21.466200,0.000000,5.317350>}
//R17 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.745000,-1.536000,6.161500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.255000,-1.536000,6.161500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<19.255000,-1.536000,6.161500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.255000,-1.536000,5.713500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<19.745000,-1.536000,5.713500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<19.255000,-1.536000,5.713500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<19.904000,-1.536000,5.932650>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-180.000000,0> translate<19.091200,-1.536000,5.932650>}
//R18 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.151000,0.000000,17.692500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.151000,0.000000,18.182500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<14.151000,0.000000,18.182500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.599000,0.000000,18.182500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<14.599000,0.000000,17.692500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<14.599000,0.000000,17.692500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<14.379850,0.000000,17.533500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<14.379850,0.000000,18.346300>}
//R19 silk screen
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<12.625000,0.000000,10.987500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<8.625000,0.000000,10.987500>}
box{<0,0,-0.063500><4.000000,0.036000,0.063500> rotate<0,0.000000,0> translate<8.625000,0.000000,10.987500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<8.625000,0.000000,10.987500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<8.625000,0.000000,15.187500>}
box{<0,0,-0.063500><4.200000,0.036000,0.063500> rotate<0,90.000000,0> translate<8.625000,0.000000,15.187500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<8.625000,0.000000,15.187500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<12.625000,0.000000,15.187500>}
box{<0,0,-0.063500><4.000000,0.036000,0.063500> rotate<0,0.000000,0> translate<8.625000,0.000000,15.187500> }
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<12.625000,0.000000,15.187500>}
cylinder{<0,0,0><0,0.036000,0>0.063500 translate<12.625000,0.000000,10.987500>}
box{<0,0,-0.063500><4.200000,0.036000,0.063500> rotate<0,-90.000000,0> translate<12.625000,0.000000,10.987500> }
difference{
cylinder{<10.625000,0,13.187500><10.625000,0.036000,13.187500>1.563500 translate<0,0.000000,0>}
cylinder{<10.625000,-0.1,13.187500><10.625000,0.135000,13.187500>1.436500 translate<0,0.000000,0>}}
//R20 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.340000,-1.536000,54.990000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<40.160000,-1.536000,54.990000>}
box{<0,0,-0.076200><0.820000,0.036000,0.076200> rotate<0,0.000000,0> translate<39.340000,-1.536000,54.990000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<39.340000,-1.536000,56.260000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<40.160000,-1.536000,56.260000>}
box{<0,0,-0.076200><0.820000,0.036000,0.076200> rotate<0,0.000000,0> translate<39.340000,-1.536000,56.260000> }
box{<-0.325000,0,-0.700000><0.325000,0.036000,0.700000> rotate<0,-0.000000,0> translate<40.481400,-1.536000,55.623500>}
box{<-0.325000,0,-0.700000><0.325000,0.036000,0.700000> rotate<0,-0.000000,0> translate<39.008200,-1.536000,55.623500>}
//R22 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<1.213500,0.000000,6.942500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<1.213500,0.000000,7.432500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<1.213500,0.000000,7.432500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<1.661500,0.000000,7.432500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<1.661500,0.000000,6.942500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<1.661500,0.000000,6.942500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<1.442350,0.000000,6.783500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<1.442350,0.000000,7.596300>}
//R23 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<2.526000,0.000000,6.942500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<2.526000,0.000000,7.432500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,90.000000,0> translate<2.526000,0.000000,7.432500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<2.974000,0.000000,7.432500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<2.974000,0.000000,6.942500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,-90.000000,0> translate<2.974000,0.000000,6.942500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<2.754850,0.000000,6.783500>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-90.000000,0> translate<2.754850,0.000000,7.596300>}
//R25 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.192500,0.000000,19.849000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.682500,0.000000,19.849000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<63.192500,0.000000,19.849000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.682500,0.000000,19.401000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.192500,0.000000,19.401000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<63.192500,0.000000,19.401000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<63.033500,0.000000,19.620150>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<63.846300,0.000000,19.620150>}
//R27 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.192500,0.000000,21.411500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.682500,0.000000,21.411500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<63.192500,0.000000,21.411500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.682500,0.000000,20.963500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.192500,0.000000,20.963500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<63.192500,0.000000,20.963500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<63.033500,0.000000,21.182650>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<63.846300,0.000000,21.182650>}
//R28 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.317500,-1.536000,2.276000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.807500,-1.536000,2.276000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<58.317500,-1.536000,2.276000> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.807500,-1.536000,2.724000>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.317500,-1.536000,2.724000>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<58.317500,-1.536000,2.724000> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<58.158500,-1.536000,2.504850>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<58.971300,-1.536000,2.504850>}
//R29 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.317500,-1.536000,1.088500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.807500,-1.536000,1.088500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<58.317500,-1.536000,1.088500> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.807500,-1.536000,1.536500>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<58.317500,-1.536000,1.536500>}
box{<0,0,-0.076200><0.490000,0.036000,0.076200> rotate<0,0.000000,0> translate<58.317500,-1.536000,1.536500> }
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<58.158500,-1.536000,1.317350>}
box{<-0.150000,0,-0.299950><0.150000,0.036000,0.299950> rotate<0,-0.000000,0> translate<58.971300,-1.536000,1.317350>}
//T1 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<61.452600,0.000000,39.152100>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<61.452600,0.000000,40.472900>}
box{<0,0,-0.076200><1.320800,0.036000,0.076200> rotate<0,90.000000,0> translate<61.452600,0.000000,40.472900> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<61.452600,0.000000,40.472900>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<64.297400,0.000000,40.472900>}
box{<0,0,-0.076200><2.844800,0.036000,0.076200> rotate<0,0.000000,0> translate<61.452600,0.000000,40.472900> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<64.297400,0.000000,40.472900>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<64.297400,0.000000,39.152100>}
box{<0,0,-0.076200><1.320800,0.036000,0.076200> rotate<0,-90.000000,0> translate<64.297400,0.000000,39.152100> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<64.297400,0.000000,39.152100>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<61.452600,0.000000,39.152100>}
box{<0,0,-0.076200><2.844800,0.036000,0.076200> rotate<0,0.000000,0> translate<61.452600,0.000000,39.152100> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<64.297400,0.000000,39.997900>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<64.297400,0.000000,39.152100>}
box{<0,0,-0.076200><0.845800,0.036000,0.076200> rotate<0,-90.000000,0> translate<64.297400,0.000000,39.152100> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<64.297400,0.000000,39.152100>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<63.532600,0.000000,39.152100>}
box{<0,0,-0.076200><0.764800,0.036000,0.076200> rotate<0,0.000000,0> translate<63.532600,0.000000,39.152100> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<61.452600,0.000000,39.152100>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<61.452600,0.000000,39.997900>}
box{<0,0,-0.076200><0.845800,0.036000,0.076200> rotate<0,90.000000,0> translate<61.452600,0.000000,39.997900> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<62.242400,0.000000,39.152100>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<61.452600,0.000000,39.152100>}
box{<0,0,-0.076200><0.789800,0.036000,0.076200> rotate<0,0.000000,0> translate<61.452600,0.000000,39.152100> }
box{<-0.228600,0,-0.292100><0.228600,0.036000,0.292100> rotate<0,-180.000000,0> translate<62.875000,0.000000,38.809200>}
box{<-0.228600,0,-0.292100><0.228600,0.036000,0.292100> rotate<0,-180.000000,0> translate<61.935200,0.000000,40.815800>}
box{<-0.228600,0,-0.292100><0.228600,0.036000,0.292100> rotate<0,-180.000000,0> translate<63.814800,0.000000,40.815800>}
//T2 silk screen
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.827600,-1.536000,55.222900>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.827600,-1.536000,53.902100>}
box{<0,0,-0.076200><1.320800,0.036000,0.076200> rotate<0,-90.000000,0> translate<34.827600,-1.536000,53.902100> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.827600,-1.536000,53.902100>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.672400,-1.536000,53.902100>}
box{<0,0,-0.076200><2.844800,0.036000,0.076200> rotate<0,0.000000,0> translate<34.827600,-1.536000,53.902100> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.672400,-1.536000,53.902100>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.672400,-1.536000,55.222900>}
box{<0,0,-0.076200><1.320800,0.036000,0.076200> rotate<0,90.000000,0> translate<37.672400,-1.536000,55.222900> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.672400,-1.536000,55.222900>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.827600,-1.536000,55.222900>}
box{<0,0,-0.076200><2.844800,0.036000,0.076200> rotate<0,0.000000,0> translate<34.827600,-1.536000,55.222900> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.672400,-1.536000,54.377100>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.672400,-1.536000,55.222900>}
box{<0,0,-0.076200><0.845800,0.036000,0.076200> rotate<0,90.000000,0> translate<37.672400,-1.536000,55.222900> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<37.672400,-1.536000,55.222900>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<36.907600,-1.536000,55.222900>}
box{<0,0,-0.076200><0.764800,0.036000,0.076200> rotate<0,0.000000,0> translate<36.907600,-1.536000,55.222900> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.827600,-1.536000,55.222900>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.827600,-1.536000,54.377100>}
box{<0,0,-0.076200><0.845800,0.036000,0.076200> rotate<0,-90.000000,0> translate<34.827600,-1.536000,54.377100> }
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<35.617400,-1.536000,55.222900>}
cylinder{<0,0,0><0,0.036000,0>0.076200 translate<34.827600,-1.536000,55.222900>}
box{<0,0,-0.076200><0.789800,0.036000,0.076200> rotate<0,0.000000,0> translate<34.827600,-1.536000,55.222900> }
box{<-0.228600,0,-0.292100><0.228600,0.036000,0.292100> rotate<0,-180.000000,0> translate<36.250000,-1.536000,55.565800>}
box{<-0.228600,0,-0.292100><0.228600,0.036000,0.292100> rotate<0,-180.000000,0> translate<35.310200,-1.536000,53.559200>}
box{<-0.228600,0,-0.292100><0.228600,0.036000,0.292100> rotate<0,-180.000000,0> translate<37.189800,-1.536000,53.559200>}
//U$1 silk screen
//U$2 silk screen
//U$3 silk screen
//U$4 silk screen
//U$5 silk screen
//U$15 silk screen
texture{col_slk}
}
#end
translate<mac_x_ver,mac_y_ver,mac_z_ver>
rotate<mac_x_rot,mac_y_rot,mac_z_rot>
}//End union
#end

#if(use_file_as_inc=off)
object{  HDMI_RGB_LVDS_V10(-35.000000,0,-30.000000,pcb_rotate_x,pcb_rotate_y,pcb_rotate_z)
#if(pcb_upsidedown=on)
rotate pcb_rotdir*180
#end
}
#end


//Parts not found in 3dpack.dat or 3dusrpac.dat are:
