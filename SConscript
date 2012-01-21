Import( 'project', 'libs' )

import os.path

project.StaticLibrary(
		'sequenceParser',
		dirs = ['src'],
		includes = ['src'],
		libraries = [
			libs.boost_filesystem,
			libs.boost_regex,
			],
		shared = True
	)


pythonOutputDir = project.inOutputDir('python/sequenceParser')

envSwig = project.createEnv( [libs.python, libs.sequenceParser, libs.pthread], name='pySequenceParser' )
envSwig.AppendUnique( SWIGFLAGS = ['-python','-c++','-Wall']) # ,'-shadow','-docstring'] )
envSwig.AppendUnique( SWIGPATH = envSwig['CPPPATH'] )
envSwig.AppendUnique( SWIGOUTDIR = pythonOutputDir )
envSwig.Replace( SHLIBPREFIX = '_' )
if project.macos:
	envSwig.Replace( SHLIBSUFFIX = '.so' ) # .dyLib not recognized

seqPython = project.SharedLibrary(
		target = 'pySequenceParser',
		sources = ['src/sequenceParser.i'],
		env = envSwig,
		installDir = pythonOutputDir,
	)

initFile = envSwig.Command( os.path.join( pythonOutputDir, '__init__.py' ), '',
                            [ Mkdir('${TARGET.dir}'),
                              Touch('$TARGET'),
                            ])
envSwig.Requires( seqPython, initFile )

envSwig.Alias( 'python', seqPython )



