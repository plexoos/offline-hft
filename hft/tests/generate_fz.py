#!/usr/bin/python

import sys
import string
import os
import math
from subprocess import call


gbl_kumac_template = """detp geometry y2014
gexec $STAR_LIB/StarMagFiledNoDict.so
gexec $STAR_LIB/xgeometry.so
make sim/gstar
gclose all
debug on
gfile o single_track_phi_{phi_deg:03d}.fz
user/vxyz 0.001 0.001 0.01
user/vsig 0.01 .1
aguser/gkine 1 5 5.0 5.0 0 0 {phi_rad:7.5f} {phi_rad:7.5f}
trig 1
quit
"""



def main():

   for angle_phi in range(60, 121, 1):
      generate_kumac(angle_phi)



def generate_kumac(angle_phi):

   phi_deg = angle_phi
   phi_rad = phi_deg/180.*math.pi

   print
   print
   print "vvv"
   print
   print "phi_deg: {0:6.3f}, phi_rad: {1:7.5f}\n".format(phi_deg, phi_rad)

   kumac_contents = gbl_kumac_template.format(**{'phi_deg': phi_deg, 'phi_rad': phi_rad})

   print "kumac_contents:\n" + kumac_contents

   my_kumac = open('my_.kumac', 'w')
   my_kumac.write(kumac_contents)
   my_kumac.close()

   cmd = "starsim -w 0 < my_.kumac\n"
   print "%s\n" % (cmd),
   os.system(cmd)



if __name__ == '__main__':
    main()
