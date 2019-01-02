/* a1691807, Yi-Ting, Hsieh */
import com.sun.net.httpserver.Headers;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;
import java.io.*;
import java.net.*;
import java.util.*;
import java.text.SimpleDateFormat;

/*****************************************************************************
*  AtomServer
*  1. Startup the AtomServer with default port:4567 by using HttpServer.
*  2. Start to listen to requests.
*****************************************************************************/
public class AtomServer {
    private static final HashMap<String, Date> feedDates = new HashMap<String, Date>();
    private static final HashMap<String, List> feedEntries = new HashMap<String, List>();

    public static void main(String[] args) throws IOException {
        int port = (args.length < 1) ? 4567 : Integer.parseInt(args[0]);
        HttpServer server = HttpServer.create(new InetSocketAddress(port), 0);
        server.createContext("/", new Handler(feedDates, feedEntries));
        server.start();
        System.out.println("Atom Server ready. Listening on port: " + port);
    }
}


/*****************************************************************************
*  Handler(HttpHandler)
*  1. Handler provide a handle methos that serves both PUT and GET requests to the server.
*  2. Responds to the remote host with appropriate content based on the request.
*  3. The Handler can only store 25 feeds at a time.
*  4. The Handler will removes the feed every 15 seconds.
*  5. Store the log file in atom.xml for backup.
*****************************************************************************/
class Handler implements HttpHandler {
    private static HashMap<String, Date> feedDates;
    private static HashMap<String, List> feedEntries;
    private static int lamportClock = 1;
    private final SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'hh:mm:ssZ");
    private final TimeZone time = TimeZone.getTimeZone("UTC");
    private Date lastUpdated = new Date();

    public Handler(HashMap<String, Date> fd, HashMap<String, List> fe) {
        feedDates = fd;
        feedEntries = fe;
        sdf.setTimeZone(time);
    }

    // The handle method serves both PUT and GET requests to the server.
    // It will respond to the remote host with appropriate content.
    // Store the log file in atom.xml
    @Override
    public void handle(HttpExchange xchg) throws IOException {
        String httpMethod = xchg.getRequestMethod();
        InetSocketAddress clientAddr = xchg.getRemoteAddress();
        System.out.println("Received " + httpMethod + " request from: " + clientAddr.getHostName() + ":" + clientAddr.getPort());

        Headers requestHeader = xchg.getRequestHeaders();
        int requestLamportClock = Integer.parseInt(requestHeader.getFirst("lamport-clock"));
        int response = HttpURLConnection.HTTP_BAD_REQUEST;
        StringBuilder responseMsg = new StringBuilder();

        //PUT
        if (httpMethod.equals("PUT") && (xchg.getRequestURI().getPath().equals("/atom.xml"))) {
            FeedProcessor feedproc = new FeedProcessor();
            response = feedproc.parseFeedDom(xchg.getRequestBody());

            if (response == HttpURLConnection.HTTP_OK) {
                lastUpdated = new Date();
                AtomFeed atomfeed = feedproc.getAtomFeed();
                String feedid = atomfeed.getFeedId();

                if (!feedDates.containsKey(feedid))
                    response = HttpURLConnection.HTTP_CREATED;

                feedDates.put(feedid, lastUpdated);
                feedEntries.put(feedid, atomfeed.getEntries());
                responseMsg.append("feed content received and processed successfully.");
                this.lamportClock = Math.max(lamportClock, requestLamportClock) + 1;
            }
        }

        //GET
        else if (httpMethod.equals("GET")) {
            removeOldEntries();

            responseMsg.append(generateXmlDom());
            response = HttpURLConnection.HTTP_OK;
            this.lamportClock = Math.max(lamportClock, requestLamportClock) + 1;
        }

        Headers responseHeader = xchg.getResponseHeaders();
        responseHeader.add("lamport-clock", lamportClock + "");
        xchg.sendResponseHeaders(response, responseMsg.length());
        OutputStream os = xchg.getResponseBody();
        os.write(responseMsg.toString().getBytes());
        os.close();

        PrintWriter pw = new PrintWriter("atom.xml");
        pw.write(generateXmlDom());
        pw.close();
    }

    // Remove any feed that was submitted more than 15 seconds ago.
    private void removeOldEntries() {
        for (Map.Entry<String, Date> feedMap : feedDates.entrySet()) {
            Date now = new Date();
            Date feedSubmitDate  = feedMap.getValue();
            long timediff = (now.getTime() - feedSubmitDate.getTime());

            if (timediff > 15000) {
                System.out.println("Removing old feed: " + feedMap.getKey());
                feedEntries.remove(feedMap.getKey());
                feedDates.remove(feedMap.getKey());
            }
        }
    }

    // Extract all entries from all the submitted feeds and sort them by date.
    // Then, it will generate a XML format String with latest 25 entries.
    private String generateXmlDom() {
        StringBuilder xml = new StringBuilder();
        xml.append("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>"
               + "\n<feed xml:lang=\"en-US\" xmlns=\"http://www.w3.org/2005/Atom\">"
               + "\n<id>http://www.cs.adelaide.edu.au.com/feed/atom</id>"
               + "\n<title>Computer Science School's News</title>"
               + "\n<subtitle>Latest news from University of Adelaide</subtitle>"
               + "\n<updated>" + sdf.format(lastUpdated).replaceAll("(\\d\\d)(\\d\\d)$", "$1:$2") + "</updated>"
               + "\n<link href=\"http://www.cs.adelaide.edu.au.com/feed/atom\" rel=\"self\">"
               + "\n</link>"
               + "\n<author>"
               + "\n<name>Yi-Ting, Hsieh</name>"
               + "\n</author>");

        if (!feedEntries.isEmpty()) {
            List<FeedEntry> allEntries = new ArrayList<FeedEntry>();
            for (Map.Entry<String, List> csMap : feedEntries.entrySet()) {
                List csList = csMap.getValue();
                Iterator itCSList = csList.iterator();
                while (itCSList.hasNext()) {
                    FeedEntry singleentry = (FeedEntry) itCSList.next();
                    allEntries.add(singleentry);
                }
            }

            Collections.sort(allEntries, new FeedEntryComparator());
            Iterator itAllEntries = allEntries.iterator();
            int entrynum = 1;
            while (itAllEntries.hasNext()) {
                FeedEntry cEntry = (FeedEntry) itAllEntries.next();
                String currentEntry = "\n<entry>";
                currentEntry += "\n<title>" + cEntry.getTitle() + "</title>";
                currentEntry += "\n<link href=\"" + cEntry.getLink() + "\"></link>";
                currentEntry += "\n<id>" + cEntry.getEntryId() + "</id>";

                String entrydate = sdf.format(cEntry.getDate()).replaceAll("(\\d\\d)(\\d\\d)$", "$1:$2");
                currentEntry += "\n<updated>" + entrydate + "</updated>";
                currentEntry += "\n<summary>" + cEntry.getSummary() + "</summary>";
                currentEntry += "\n</entry>";
                xml.append(currentEntry);

                if (entrynum == 25)
                    break;

                entrynum++;
            }
        }
        xml.append("\n</feed>");
        return xml.toString();
    }
}
