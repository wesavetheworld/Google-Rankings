; $Id$

Google rankings module gives you detailed statistics about the websites positions on Google results pages. This is achieved by analyzing HTTP referers headers set by Google web search.

There are two types of referes. The old referer format (google.com/search) carries information about the queriy string and the page number, the link was in. But the new referer format (google.com/url) besides the query string also contains information about the position of the link, on Google SERP. At the moment, about 25%-30% of the traffic sent by Google has the new referer format, and 70%-75% has the old format.

Dependencies:

This module relies on core Statistics module to be enabled and configured to collect access log information. If you are using a module (like Boost for example) or any other type of software configuration which breaks the anonymous usage statistics, than this module will not work.
