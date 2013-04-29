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


pythonOutputDir = project.inOutputDir('python/pySequenceParser')

envSwig = project.createEnv( [
	libs.python,
	libs.pthread,
	libs.sequenceParser,
	], name='pySequenceParser' )
envSwig.AppendUnique( SWIGFLAGS = ['-python','-c++','-Wall'] )
envSwig.AppendUnique( SWIGPATH = envSwig['CPPPATH'] )
envSwig.AppendUnique( SWIGOUTDIR = pythonOutputDir )
envSwig.Replace( SHLIBPREFIX = '' )
if project.macos:
	envSwig.Replace( SHLIBSUFFIX = '.so' ) # .dyLib not recognized
if project.windows:
	envSwig.Replace( SHLIBSUFFIX = '.pyd' ) # .dyLib not recognized

seqPython = project.SharedLibrary(
		target = '_sequenceParser',
		sources = ['src/sequenceParser.i'],
		env = envSwig,
		installDir = pythonOutputDir,
	)

initFile = envSwig.Command( os.path.join( pythonOutputDir, '__init__.py' ), '',
                            [ Mkdir('${TARGET.dir}'),
                              Touch('$TARGET'),
                            ])
envSwig.Requires( seqPython, initFile )

envSwig.Alias( 'pySequenceParser', seqPython )
envSwig.Alias( 'python', seqPython )



