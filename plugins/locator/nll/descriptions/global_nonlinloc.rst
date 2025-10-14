Funded by `SED/ETH Zurich <http://www.seismo.ethz.ch/>`_, developed by `gempa GmbH <http://www.gempa.de>`_.
This plugin is available from SeisComP version Release Potsdam 2010 and later.

The `NonLinLoc (NLL) <http://alomax.free.fr/nlloc>`_ locator algorithm has been
implemented into SeisComP through the plugin mechanism. A new plugin locnll
contains the LocatorInterface implementation for NonLinLoc.

The implementation bundles the NonLinLoc source files required to use the library
function calls. The following source files are included:

.. code-block:: sh

   GridLib.c
   GridLib.h
   GridMemLib.c
   GridMemLib.h
   NLLocLib.h
   NLLoc1.c
   NLLocLib.c
   calc_crust_corr.c
   calc_crust_corr.h
   crust_corr_model.h
   crust_type_key.h
   crust_type.h
   loclist.c
   octtree.h
   octtree.c
   phaseloclist.h
   phaselist.c
   geo.c
   geo.h
   geometry.h
   map_project.c
   map_project.h
   otime_limit.c
   otime_limit.h
   ran1.c
   ran1.h
   velmod.c
   velmod.h
   util.h
   util.c
   alomax_matrix/alomax_matrix.c
   alomax_matrix/alomax_matrix.h
   alomax_matrix/alomax_matrix_svd.c
   alomax_matrix/alomax_matrix_svd.h


Error measures
==============

After running NonLinLoc the output is converted into a SeisComP (QuakeML) origin
object including all available error measures. The following table shows how
the NLL error measures are mapped to the SeisComP data model:

=========================================================  =====================================================
SeisComP                                                   NLL
=========================================================  =====================================================
Origin.latitude.uncertainty                                sqrt(phypo->cov.yy)
Origin.longitude.uncertainty                               sqrt(phypo->cov.xx)
Origin.depth.uncertainty                                   sqrt(phypo->cov.zz)
Origin.originQuality.standardError                         phypo->rms
Origin.originQuality.secondaryAzimuthalGap                 phypo->gap_secondary
Origin.originQuality.usedStationCount                      phypo->usedStationCount
Origin.originQuality.associatedStationCount                phypo->associatedStationCount
Origin.originQuality.associatedPhaseCount                  phypo->associatedPhaseCount
Origin.originQuality.usedPhaseCount                        phypo->nreadings
Origin.originQuality.depthPhaseCount                       phypo->depthPhaseCount
Origin.originQuality.minimumDistance                       km2deg(phypo->minimumDistance)
Origin.originQuality.maximumDistance                       km2deg(phypo->maximumDistance)
Origin.originQuality.medianDistance                        km2deg(phypo->medianDistance)
Origin.originQuality.groundTruthLevel                      phypo->groundTruthLevel
Origin.originUncertainty.horizontalUncertainty             phypo->ellipse.len2
Origin.originUncertainty.minHorizontalUncertainty          phypo->ellipse.len1
Origin.originUncertainty.maxHorizontalUncertainty          phypo->ellipse.len2
Origin.originUncertainty.azimuthMaxHorizontalUncertainty   phypo->ellipse.az1 + 90
ConfidenceEllipsoid.semiMajorAxisLength                    phypo->ellipsoid.len3
ConfidenceEllipsoid.semiMinorAxisLength                    phypo->ellipsoid.len1
ConfidenceEllipsoid.semiIntermediateAxisLength             phypo->ellipsoid.len2
ConfidenceEllipsoid.majorAxisPlunge                        (phypo->ellipsoid.axis1 x phypo->ellipsoid.axis2).dip
ConfidenceEllipsoid.majorAxisAzimuth                       (phypo->ellipsoid.axis1 x phypo->ellipsoid.axis2).az
ConfidenceEllipsoid.majorAxisRotation                      T.B.D.
=========================================================  =====================================================


Plugin
======

The NonLinLoc plugin is installed under :file:`share/plugins/locnll.so`.
It provides a new implementation of the LocatorInterface with the name NonLinLoc.

To add the plugin to a module add it to the modules configuration, either
:file:`modulename.cfg` or :file:`global.cfg`:

.. code-block:: sh

   plugins = ${plugins}, locnll

Basically it can be used by two modules: :ref:`screloc` and :ref:`scolv`.


Output
======

All output is stored in the configured :confval:`NonLinLoc.outputPath`.
The file prefix for a location is the originID (:confval:`NonLinLoc.publicID`).

The following file are stored:

- Input observations (.obs)
- Input configuration (.conf)
- NLL location (.loc.hyp)
- NLL 3D grid header (.loc.hdr)
- NLL octree (.loc.octree)
- NLL scatter file (.loc.scat)

In addition to the native NLL output a SeisComP origin object is created and
returned to the calling instance. Usually this object is then sent via messaging.


Profiles
========

The plugin allows to specify multiple configuration profiles (:confval:`NonLinLoc.profiles`).
The profile to use can selected both in `scolv` and `screloc`, however a 
virtual profile automatic is also provided, which selects the best matching 
configured profile based on the initial location. For this reason each profile 
contains some configuration parameters that defines where the profile is valid 
(`transform`, `region`, `origin`, `rotation`). The `transform` profile 
configuration parameter supports only `GLOBAL` or `SIMPLE` at the moment: only the
profile has this limitation, not the NonLinLoc control file, which supports 
all transformations available in NonLinLoc.

**NOTE**: If a profile `transform` is set as `GLOBAL` and the `region` parameter is
left `empty`, then the plugin adds the line `TRANS GLOBAL` to the control file,
forcing a global transformation.


Configuration example
=====================

To add the plugin to an application such as scolv or screloc, add the plugin
name to the list of plugins that are loaded (e.g. :file:`scolv.cfg`):

.. code-block:: sh

   plugins = ${plugins}, locnll


Futhermore add the plugin configuration (e.g. :file:`scolv.cfg`):

.. code-block:: sh

   ########################################################
   ################ NonLinLoc configuration################
   ########################################################
   NLLROOT = ${HOME}/nll/data

   NonLinLoc.outputPath = ${NLLROOT}/output/

   # Define the default control file if no profile specific
   # control file is defined.
   NonLinLoc.controlFile = ${NLLROOT}/NLL.default.conf

   # Set the default pick error in seconds passed to NonLinLoc
   # if no SeisComP pick uncertainty is available.
   NonLinLoc.defaultPickError = 0.1

   # Define the available NonLinLoc location profiles. The order
   # implicitly defines the priority for overlapping regions
   #NonLinLoc.profiles = swiss_3d, swiss_1d, global
   NonLinLoc.profiles = swiss_3d, global

   # The earthModelID is copied to earthModelID attribute of the
   # resulting origin
   NonLinLoc.profile.swiss_1d.earthModelID = "swiss regional 1D"

   # Specify the velocity model table path as used by NonLinLoc
   NonLinLoc.profile.swiss_1d.tablePath = ${NLLROOT}/time_1d_regio/regio

   # Specify the region valid for this profile
   # Without this parameter the plugin will add an additional
   # TRANS GLOBAL statement in the NLL control file
   NonLinLoc.profile.swiss_1d.region = 41.2, 3.8, 50.1, 16.8

   # The NonLinLoc  control file to use for this profile
   NonLinLoc.profile.swiss_1d.controlFile = ${NLLROOT}/NLL.swiss_1d.conf

   # Configure the swiss_3d profile
   NonLinLoc.profile.swiss_3d.earthModelID = "swiss regional 3D"
   NonLinLoc.profile.swiss_3d.tablePath = ${NLLROOT}/time_3d/ch
   NonLinLoc.profile.swiss_3d.region = 45.15, 5.7, 48.3, 11.0
   NonLinLoc.profile.swiss_3d.controlFile = ${NLLROOT}/NLL.swiss_3d.conf

   # And the global profile
   NonLinLoc.profile.global.earthModelID = iaspei91
   NonLinLoc.profile.global.tablePath = ${NLLROOT}/iasp91/iasp91
   NonLinLoc.profile.global.controlFile = ${NLLROOT}/NLL.global.conf


An example of a NonLinLoc control file configuration that contains all the
required statements, but it must be adapted to the specific use case. The
missing statements are generated by the plugin (LOCFILES, LOCHYPOUT, LOCSRCE):

.. code-block:: sh

  # -1 = no logs, useful for playback with screloc --ep option
  CONTROL -1 123456
  # This must be the same TRANS used for generating the grid files
  TRANS SDC 46.51036987 8.47575546 0.0
  LOCSIG Swiss Seismological Service, ETHZ
  LOCCOM location using my local velocity model
  LOCSEARCH OCT 20 20 20 0.001 10000 1000
  # This grid origin is relative to the TRANS statement. The grid
  # must be wholly contained in the grid files
  LOCGRID 101 101 101 -0.5 -0.5 -1.8  0.01 0.01 0.01 PROB_DENSITY SAVE
  LOCMETH EDT_OT_WT 9999.0 4 -1 -1 -1 0 -1 1
  LOCGAU 0.001 0.0
  LOCPHASEID  P   P p G Pn Pg P1
  LOCPHASEID  S   S s G Sn Sg S1
  LOCQUAL2ERR 0.025 0.050 0.100 0.200 0.400 99999.9
  LOCANGLES ANGLES_YES 5

**NOTE**: The LOCHYPOUT parameter statement is always generated by the plugin. 
By default it outputs `LOCHYPOUT NONE`.  If `enableSEDParameters` is enabled 
or if the original control file contains `CALC_SED_ORIGIN`, it will append 
`CALC_SED_ORIGIN`.  If the original control file contains `SAVE_NLLOC_EXPECTATION`, 
that flag will also be preserved. Currently, only `CALC_SED_ORIGIN` and 
`SAVE_NLLOC_EXPECTATION` are supported by the plugin. Any other options are 
omitted and will not be forwarded to NonLinLoc.

Stations names
============== 

When generating the grid files the station names used in the GTSRCE statement
must match the rule set in the plugin configuration. E.g.

.. code-block:: sh

  # Format of the station name used to select the right travel time table (grid)
  # file for a station. By default only the station code is used (e.g.
  # tablePath.P.@STA@.time.*), but that doesn't allow to distinguish between
  # multiple network codes or location codes that use the same station code. To
  # overcome this limitation this parameter could be set in a more general way,
  # for example @NET@_@STA@_@LOC@. In this way NonLinLoc will look for travel
  # time table (grid) files of the form: tablePath.P.@NET@_@STA@_@LOC@.time.*
  # Where @NET@ @STA@ @LOC@ are just placeholder for the actual codes
  NonLinLoc.profile.MYPROFILE.stationNameFormat = @NET@_@STA@_@LOC@


Given the above plugin configuration, the GTSRCE statement should be something
like this:

.. code-block:: sh

  GTSRCE CH_STA01_   LATLON  46.519  8.474  0.0  1.295
  GTSRCE CH_STA02_01 LATLON  46.456  8.474  0.0  1.323
  GTSRCE CH_STA03_AA LATLON  46.784  8.474  0.0  1.292


Alternatively the names could just contain the station code: 

.. code-block:: sh

  NonLinLoc.profile.MYPROFILE.stationNameFormat = @STA@ 

.. code-block:: sh

  GTSRCE STA01 LATLON  46.519  8.474  0.0  1.295
  GTSRCE STA02 LATLON  46.456  8.474  0.0  1.323
  GTSRCE STA03 LATLON  46.784  8.474  0.0  1.292


Usage
=====

Locator
-------

The usage of the new NLL plugin is straight forward. Once loaded successfully the
new locator shows up in the lower left corners combo box.

.. figure:: media/nonlinloc/locator_selection_small.png

Select the new NonLinLoc locator and the configured profiles will be loaded into
the combo box right of it.

.. figure:: media/nonlinloc/locator_profile_selection_small.png

The NonLinLoc implementation provides a virtual profile automatic. This emulates
the complete automatic case and selects the best matching configured profiles
based on the initial location.

If an origin has been relocated the method should be set to "NonLinLoc" and
the earth model contains the string NonLinLoc.profile.[name].earthModelID
configured for the selected profile.

.. figure:: media/nonlinloc/origin_information.png


Settings
--------

The NLL locator implementation supports to override configured settings or
control parameters for a session. Those changes are not persistent and lost if
the locator is changed to another one or the profile has been changed.  However
this feature is particularly useful when trying differnt settings on a particular
origin or for enabling the NonLinLoc logs (`CONTROL` statement) that becomes
visible on the console.

To open the settings dialog press the button right to the locator selection
combo box.

.. figure:: media/nonlinloc/locator_settings.png

Then the NLL specific parameters show up.

.. figure:: media/nonlinloc/NLL_settings.png


Seismicity Viewer
-----------------

scolv provides two additional configurable buttons. To bind
`Seismicity Viewer <http://alomax.free.fr/seismicity>`_ to the first one the
following configuration can be used:

.. code-block:: sh

   button0 = "Seismicity Viewer"
   scripts.script0 = @CONFIGDIR@/scripts/sv

A small wrapper script sv has been created that calls Seismicity Viewer based
on the origin ID passed to the script.

.. code-block:: sh

   #!/bin/sh
   FILE=$HOME/nll/data/output/$1.loc.hyp
   java -classpath $HOME/nll/bin/SeismicityViewer50.jar \
        net.alomax.seismicity.Seismicity $FILE

This examples assumes that Seismicity Viewer has been installed in $HOME/nll/bin.
