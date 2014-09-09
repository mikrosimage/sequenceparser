Import( 'project', 'libs' )

import os.path

project.StaticLibrary(
		'sequenceParser',
		dirs = ['src'],
		includes = ['src'],
		libraries = [
			libs.boost_filesystem,
			libs.boost_regex,
			libs.boost_locale,
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

SConscript([
	'pyTest/SConscript',
	'test/filesDetection/SConscript',
	'test/stringSequenceDetection/SConscript',
	])

