load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/contributed.ncl"
load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/gsn_code.ncl"
load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/gsn_csm.ncl"
   
begin

header = readAsciiHead("value", 3);                         src/dst grid and test function information
srcname = header(0);
dstname = header(1);
testfunction = header(2);
ncfile = str_concat((/srcname, "_", dstname, ".nc"/));
;print(srcname);
;print(dstname);
;print(testfunction);
;print(ncfile);

values = readAsciiTable("value", 7, "float", (/4, 0/))  ;   values on grid
;printVarSummary(values)

cdf = addfile(ncfile, "r")                        ;   open netCDF file for lat/lon information
dst_dims = cdf->dst_grid_dims;
;printVarSummary(dims)
nlat = dst_dims(1);
nlon = dst_dims(0);
;print(nlat);
;print(nlon);

rad2deg = 180 / 3.14159265359;
lat1d = cdf->dst_grid_center_lat * rad2deg;
lon1d = cdf->dst_grid_center_lon * rad2deg;
lat2d = onedtond(lat1d, (/nlat, nlon/));
lon2d = onedtond(lon1d, (/nlat, nlon/));

; lat/lon for drawing
lats=lat2d(:,0);
lons=lon2d(0,:);
lats@units="degrees_north";
lons@units="degrees_east";

; values to draw
af=values(:,0);
o1=values(:,1);
o2a=values(:,2);
o2d=values(:,3);
o1_err=values(:,4);
o2a_err=values(:,5);
o2d_err=values(:,6);
o1_af=o1-af;
o2a_af=o2a-af;
o2d_af=o2d-af;
o2d_o2a=o2d-o2a;

; from 1D to 2D
af2d = onedtond(af, (/nlat, nlon/));
af2d@lat2d=lat2d;
af2d@lon2d=lon2d;

o1_af2d = onedtond(o1_af, (/nlat, nlon/));
o1_af2d@lat2d=lat2d;
o1_af2d@lon2d=lon2d;

o2a_af2d = onedtond(o2a_af, (/nlat, nlon/));
o2a_af2d@lat2d=lat2d;
o2a_af2d@lon2d=lon2d;

o2d_af2d = onedtond(o2d_af, (/nlat, nlon/));
o2d_af2d@lat2d=lat2d;
o2d_af2d@lon2d=lon2d;

o2d_o2a2d = onedtond(o2d_o2a, (/nlat, nlon/));
o2d_o2a2d@lat2d=lat2d;
o2d_o2a2d@lon2d=lon2d;

o1_err2d = onedtond(o1_err, (/nlat, nlon/));
o1_err2d@lat2d=lat2d;
o1_err2d@lon2d=lon2d;

o2a_err2d = onedtond(o2a_err, (/nlat, nlon/));
o2a_err2d@lat2d=lat2d;
o2a_err2d@lon2d=lon2d;

o2d_err2d = onedtond(o2d_err, (/nlat, nlon/));
o2d_err2d@lat2d=lat2d;
o2d_err2d@lon2d=lon2d;

print("Finish data preparation");
; create plot: reference="www.ncl.ucar.edu/Applications/Scripts/proj_1.ncl"

res                     = True;
res@mpProjection        = "CylindricalEquidistant";
res@mpGridAndLimbOn     = True;
res@mpPerimOn           = False;
res@mpGridLatSpacingF   = 30.;
res@mpGridLonSpacingF   = 30.;
res@mpFillOn            = False;

res@mpLimitMode         = "Corners";        choose range of map
res@mpLeftCornerLatF    = - 90.0;
res@mpLeftCornerLonF    = - 180.0;
res@mpRightCornerLatF   =   90.0;
res@mpRightCornerLonF   =   180.0;

res@mpRelativeCenterLon = True;
res@mpCenterLonF        = 0;
res@mpCenterLatF        = 0;
res@tfDoNDCOverlay      = False;
res@cnFillOn            = True;
;res@cnLineLabelOn       = False;
res@cnLinesOn           = False;
res@txFontHeightF       = 0.015;

res@vpXF                = 0.1;
res@vpYF                = 0.9;
res@vpWidthF            = 0.8;
res@vpHeightF           = 0.9;

res@gsnSpreadColors     = True;
res@gsnSpreadColorStart = 2;
res@gsnSpreadColorEnd   = -3;
res@lbLabelFontHeightF  = 0.008
res@lbLabelAngleF       = 45;

res@tiMainFontHeightF   = 0.018

wks1 = gsn_open_wks("ps", str_concat((/srcname, "_", dstname, ".", testfunction, ".af"/)));
wks2 = gsn_open_wks("ps", str_concat((/srcname, "_", dstname, ".", testfunction, ".o1_af"/)));
wks3 = gsn_open_wks("ps", str_concat((/srcname, "_", dstname, ".", testfunction, ".o2a_af"/)));
wks4 = gsn_open_wks("ps", str_concat((/srcname, "_", dstname, ".", testfunction, ".o2d_af"/)));
wks5 = gsn_open_wks("ps", str_concat((/srcname, "_", dstname, ".", testfunction, ".o2d_o2a"/)));
wks6 = gsn_open_wks("ps", str_concat((/srcname, "_", dstname, ".", testfunction, ".o1_err"/)));
wks7 = gsn_open_wks("ps", str_concat((/srcname, "_", dstname, ".", testfunction, ".o2a_err"/)));
wks8 = gsn_open_wks("ps", str_concat((/srcname, "_", dstname, ".", testfunction, ".o2d_err"/)));

gsn_define_colormap(wks1, "gui_default");
gsn_define_colormap(wks2, "gui_default");
gsn_define_colormap(wks3, "gui_default");
gsn_define_colormap(wks4, "gui_default");
gsn_define_colormap(wks5, "gui_default");
gsn_define_colormap(wks6, "gui_default");
gsn_define_colormap(wks7, "gui_default");
gsn_define_colormap(wks8, "gui_default");

print("Draw graph 1");
res@tiMainString        = str_concat((/"Analytic function of ", testfunction, " (af) (", dstname, ")"/));
contour = gsn_csm_contour_map(wks1, af2d(:,:), res);

print("Draw graph 2");
res@tiMainString        = str_concat((/"Order1 remap errors (o1 - af) (", srcname, " ==> ", dstname, ")"/));
contour = gsn_csm_contour_map(wks2, o1_af2d(:,:), res);

print("Draw graph 3");
res@tiMainString        = str_concat((/"Order2 remap errors with analytic gradients (o2a - af) (", srcname, " ==> ", dstname, ")"/));
contour = gsn_csm_contour_map(wks3, o2a_af2d(:,:), res);

print("Draw graph 4");
res@tiMainString        = str_concat((/"Order2 remap errors with discrete gradients (o2d - af) (", srcname, " ==> ", dstname, ")"/));
contour = gsn_csm_contour_map(wks4, o2d_af2d(:,:), res);

print("Draw graph 5");
res@tiMainString        = str_concat((/"Differences of order2 remap results (o2d - o2a) (", srcname, " ==> ", dstname, ")"/));
contour = gsn_csm_contour_map(wks5, o2d_o2a2d(:,:), res);

print("Draw graph 6");
res@tiMainString        = str_concat((/"Order1 remap errors (o1 - af)/af (", srcname, " ==> ", dstname, ")"/));
contour = gsn_csm_contour_map(wks6, o1_err2d(:,:), res);

print("Draw graph 7");
res@tiMainString        = str_concat((/"Order2 remap errors with analytic gradients (o2a - af)/af (", srcname, " ==> ", dstname, ")"/));
contour = gsn_csm_contour_map(wks7, o2a_err2d(:,:), res);

print("Draw graph 8");
res@tiMainString        = str_concat((/"Order2 remap errors with discrete gradients (o2d - af)/af (", srcname, " ==> ", dstname, ")"/));
contour = gsn_csm_contour_map(wks8, o2d_err2d(:,:), res);
end
