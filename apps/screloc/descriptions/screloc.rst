screloc is an automatic relocator that receives origins from realtime
locators such as scautoloc and relocates them with a configurable locator.
screloc can be conveniently used to test different locators and velocity models
or to relocate events with updated velocity models. Check the
:ref:`Example applications <screloc-example>` for screloc.

screloc processes any incoming automatic origin but does not yet listen to event
information nor does it skip origins for that a more recent one exists.

To run screloc along with all processing modules add it to the list of
clients in the seiscomp configuration frontend.


.. code-block:: sh

   seiscomp enable screloc
   seiscomp start screloc

Descriptions of parameters for screloc:

.. code-block:: sh

   seiscomp exec screloc -h

Test the performance of screloc and learn from debug output:

.. code-block:: sh

   seiscomp exec screloc --debug

Setup
=====

The following example configuration shows a setup of screloc for
:ref:`NonLinLoc <global_nonlinloc>`:

.. code-block:: sh

   plugins = ${plugins}, locnll

   # Define the locator algorithm to use
   reloc.locator = NonLinLoc

   # Define a suffix appended to the publicID of the origin to be relocated
   # to form the new publicID.
   # This helps to identify pairs of origins before and after relocation.
   # However, new publicIDs are unrelated to the time of creation.
   # If not defined, a new publicID will be generated automatically.
   reloc.originIDSuffix = "#relocated"

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
   NonLinLoc.profile.swiss_1d.region = 41.2, 3.8, 50.1, 16.8

   # The NonLinLoc default control file to use for this profile
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


.. _screloc-example:

Examples
========

* Relocate all origins given in an :term:`SCML` file according to the
  configuration of :program:`screloc`. Write all output to unformatted SCML.

  .. code-block:: sh

     screloc -d localhost --ep origins.xml > origins_screloc.xml

* Relocate the previously preferred origins of all events (:ref:`scevtls`)
  within some period of time using a specific :ref:`locator <concepts_locators>`
  and locator profile.
  Use some userID and authorID for uniquely recognizing the relocation.
  Configuring the ref:`scevent` parameter :confval:`eventAssociation.priorities`
  to TIME_AUTOMATIC before running the example will prefer the latest origin
  (which will be created by screloc) for the event the new origin is associated
  to. The new origins are automatically sent to the messaging.

  .. code-block:: sh

    #!/bin/bash

    # locator type
    locator=[your_locator]
    # locator profile
    profile=[your_profile]
    # set some userID
    userID=[your_user]
    # set some authorID
    authorID=[screloc]

    IFS=',' read -ra events <<< `scevtls -d localhost -p -D , --begin 2025-01-01 --end 2025-02-01`
    for event in "${events[@]}"; do
        preferredOrigin=$(echo $event | awk '{print $2}')
        screloc -d localhost -O $preferredOrigin --locator $locator --profile $profile -u $userID --author=$authorID
    done
