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

pySequenceParser = project.pySwigBinding(
			'pySequenceParser',
			'sequenceParser',
			sources=['src/sequenceParser.i'],
			libraries=[libs.sequenceParser],
			)

javaSequenceParser = project.javaSwigBinding(
			'eu.sequence.parser',
			'sequenceParser',
			sources=['src/sequenceParser.i'],
			libraries=[libs.sequenceParser],
			)

