Part of the :ref:`VS` package.

scenvelope is part of a new SeisComP implementation of the
`Virtual Seismologist`_ (VS) Earthquake Early Warning algorithm (Cua, 2005; Cua and Heaton, 2007) released
under the `SED Public License for SeisComP Contributions`_. It generates
real-time envelope values for horizontal and vertical acceleration, velocity and
displacement from raw acceleration and velocity waveforms. It was implemented
to handle the waveform pre-processing necessary for the :ref:`scvsmag` module.
It provides in effect continuous real-time streams of PGA, PGV and PGD values which
could also be used independently of :ref:`scvsmag`.

The processing procedure is as follows:

#. gain correction
#. baseline correction
#. high-pass filter with a corner frequency of 3 s period
#. integration or differentiation to velocity, acceleration and displacement
#. computation of the absolute value within 1 s intervals

The resulting envelope values are sent as messages to :ref:`scmaster`. Depending
on the number of streams that are processed this can result in a significant
number of messages (#streams/s).

scenvelope sends messages of type "VS" which requires all modules receiving these
messages to load the plugin "dmvs". This can be most easily configured through 
the configuration parameter in :file:`scenvelope.cfg`:

.. code-block:: sh

   plugins = ${plugins}, dmvs

References
==========

.. target-notes::

.. _`Virtual Seismologist` : http://www.seismo.ethz.ch/en/research-and-teaching/products-software/EEW/Virtual-Seismologist/
.. _`SED Public License for SeisComP Contributions` : http://www.seismo.ethz.ch/static/seiscomp_contrib/license.txt
