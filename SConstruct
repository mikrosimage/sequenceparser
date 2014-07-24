
EnsureSConsVersion( 1, 2 )
EnsurePythonVersion( 2, 5 )

import os
import sys

sys.path.append('tools')
from sconsProject import SConsProject

# Create the object available in all SConscripts
project = SConsProject()
Export('project')
Export({'libs':project.libs})

# Load SConscript files
project.begin()
project.SConscript()
project.end()


