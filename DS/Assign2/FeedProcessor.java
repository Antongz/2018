/* a1691807, Yi-Ting, Hsieh */
import java.io.*;
import java.net.HttpURLConnection;
import java.text.SimpleDateFormat;
import java.util.*;
import javax.xml.parsers.*;
import org.w3c.dom.*;

/*****************************************************************************
*  FeedProcessor
*  1. Provide a XML parser to read from an InputStream and store it to atomfeed.
*  2. Provide a function to get the atomfeed entity.
*****************************************************************************/
public class FeedProcessor {
    SimpleDateFormat sdf = new SimpleDateFormat("yyyy/MMM/dd HH:mm:ss z");
    TimeZone timezone = TimeZone.getTimeZone("UTC");
    AtomFeed atomfeed = null;

    public int parseFeedDom(InputStream is) {
        DocumentBuilderFactory builderFactory = DocumentBuilderFactory.newInstance();

        DocumentBuilder builder = null;
        try {
            if (is.available() < 1)
                return HttpURLConnection.HTTP_NO_CONTENT;

            builder = builderFactory.newDocumentBuilder();
            Document doc = builder.parse(is);
            doc.getDocumentElement().normalize();
            Element rootElement = doc.getDocumentElement();

            if (!rootElement.getAttribute("xmlns").matches(".*www.w3.org/2005/Atom$"))
                return HttpURLConnection.HTTP_INTERNAL_ERROR;

            String feedTitle = rootElement.getElementsByTagName("title").item(0).getTextContent();
            String feedSubTitle = rootElement.getElementsByTagName("subtitle").item(0).getTextContent();
            String feedLink = getNodeAttr("href", rootElement.getElementsByTagName("link").item(0));
            String feedDate = rootElement.getElementsByTagName("updated").item(0).getTextContent();
            String feedAuthor = rootElement.getElementsByTagName("name").item(0).getTextContent();
            String feedId = rootElement.getElementsByTagName("id").item(0).getTextContent();

            atomfeed = new AtomFeed(feedTitle, feedSubTitle, feedLink, feedDate, feedAuthor, feedId);
            NodeList entryList = doc.getElementsByTagName("entry");

            if (entryList.getLength() > 0) {
                for (int index = 0; index < entryList.getLength(); index++) {
                    Node entryNode = entryList.item(index);
                    if (entryNode.getNodeType() == Node.ELEMENT_NODE) {
                        Element entryElement = (Element) entryNode;
                        String etitle = entryElement.getElementsByTagName("title").item(0).getTextContent();
                        String elink = getNodeAttr("href", entryElement.getElementsByTagName("link").item(0));
                        String eid = entryElement.getElementsByTagName("id").item(0).getTextContent();
                        String edate = entryElement.getElementsByTagName("updated").item(0).getTextContent();
                        String esum = entryElement.getElementsByTagName("summary").item(0).getTextContent();

                        FeedEntry fe = new FeedEntry(etitle, elink, eid, edate, esum);
                        atomfeed.addEntry(fe);
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return HttpURLConnection.HTTP_OK;
    }

    // Return the AtomFeed entity
    public AtomFeed getAtomFeed() { return this.atomfeed; }

    // Return the node attribute in String type
    private String getNodeAttr(String attrName, Node node) {
        NamedNodeMap attrs = node.getAttributes();
        for (int x = 0; x < attrs.getLength(); x++) {
            Node attr = attrs.item(x);
            if (attr.getNodeName().equalsIgnoreCase(attrName))
                return attr.getNodeValue();
        }

        return "";
    }

    // Convert InputStream to string
    private String inputStream2String(InputStream is) {
        BufferedReader br = null;
        StringBuilder sb = new StringBuilder();

        String line;
        try {
            br = new BufferedReader(new InputStreamReader(is));
            while((line = br.readLine()) != null)
                sb.append(line + "\n");

        } catch (IOException ie) {
            ie.printStackTrace();
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException ie) {
                    ie.printStackTrace();
                }
            }
        }
        return sb.toString();
    }

}
