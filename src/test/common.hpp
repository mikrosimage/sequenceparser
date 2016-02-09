#ifndef COMMON_HPP
#define COMMON_HPP

#include <sequenceParser/common.hpp>
#include <sequenceParser/Item.hpp>
#include <sequenceParser/Sequence.hpp>
#include <sequenceParser/filesystem.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <vector>


std::vector<sequenceParser::Sequence> sequenceFromFilenameList(const std::vector<boost::filesystem::path>& paths,
        const sequenceParser::EDetection detectOptions = sequenceParser::eDetectionDefault)
{
    std::vector<sequenceParser::Sequence> listSequence;
    BOOST_FOREACH( const boost::filesystem::path& path, paths )
	{
        std::vector<sequenceParser::Item> items = sequenceParser::browse( path, detectOptions );
        BOOST_FOREACH( const sequenceParser::Item& item, items )
        {
            switch(item.getType())
            {
                case sequenceParser::eTypeSequence:
                    listSequence.push_back(item.getSequence());
                    break;
                default:
                    break;
            }
        }
    }
    return listSequence;
}

#endif
