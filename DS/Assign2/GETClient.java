/* a1691807, Yi-Ting, Hsieh */
import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.*;

/*****************************************************************************
*  GETClient
*  1. Startup the GETClient, read the AtomServere address from command line argument.
*  2. Connects to the AtomServer.
*  3. Send GET message to AtomServer for the feeds.
*  4. Get the feed file and display it.
*****************************************************************************/
public class GETClient {
    private static int lamportClock = 1;
    private static SimpleDateFormat sdf = new SimpleDateFormat("yyy/MMM/dd HH:mm:ss: z");
    private static TimeZone time = TimeZone.getTimeZone("UTC");

    public static void main(String[] args) {
        GETClient gc = new GETClient();
        try {
            if (args.length < 1) {
                BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
                System.out.println("Enter ATOMServer Address: ");
                gc.request(br.readLine().trim());
            } else
                gc.request(args[0]);
        } catch (IOException ie) {
            ie.printStackTrace();
        }
    }

    // Send a GET request for the Atom feed
    public void request(String url) throws IOException {
        if (!url.matches("^http://.*$"))
            url = "http://" + url;

        URL urlObj = new URL(url);
        HttpURLConnection con = (HttpURLConnection) urlObj.openConnection();
        con.setRequestMethod("GET");
        con.setRequestProperty("Connection", "close");
        con.setRequestProperty("lamport-clock", lamportClock + "");
        con.setDoOutput(true);
        con.connect();

        if (con.getResponseCode() == HttpURLConnection.HTTP_OK) {
            int lamportServer = Integer.parseInt(con.getHeaderField("lamport-clock"));

            this.lamportClock = Math.max(this.lamportClock, lamportServer) + 1;
        }

        FeedProcessor fp = new FeedProcessor();
        int parser = fp.parseFeedDom(con.getInputStream());
        con.disconnect();

        if (parser == HttpURLConnection.HTTP_OK)
            printFeed(fp.getAtomFeed());
        else
            System.err.println("Feed parsing error!");

    }

    // Display the feed content.
    public void printFeed(AtomFeed af) {
        sdf.setTimeZone(time);
        System.out.println("\nFeed: " + af.getTitle() + " - " + af.getSubtitle());
        System.out.println("Updated: " + sdf.format(af.getDate()));
        System.out.println("Link: " + af.getLink());
        System.out.println("Author: " + af.getAuthor());
        System.out.println("");

        if (!af.getEntries().isEmpty()) {
            Iterator entriesIterator = af.getEntries().iterator();

            int entryCount = 1;
            while (entriesIterator.hasNext()) {
                FeedEntry fe = (FeedEntry) entriesIterator.next();
                System.out.println("Title: " + fe.getTitle());
                System.out.println("Source: " + fe.getLink());
                System.out.println("Last Updated: " + sdf.format(fe.getDate()));
                System.out.println("-------------------------");
                System.out.println(fe.getSummary());
                System.out.println("");
            }
        } else
            System.out.println("The feed doesn't contain any news item");
    }
}
