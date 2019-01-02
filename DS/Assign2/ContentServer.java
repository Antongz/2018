/* a1691807, Yi-Ting, Hsieh */
import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;

/*****************************************************************************
*  ContentServer
*  1. Startup the ContentServer, read 2 arguments from command for file and the url.
*  2. Connects to the AtomServer.
*  3. Parses the file content and sumbit the atom feed to AtomServer(PUT request).
*  4. Detects any modification to the news content file and re-publish the feed(send the PUT message again).
*  5. Has 5 times retry on error functionality.
*****************************************************************************/
public class ContentServer {
    private static int lamportClock = 1;

    public static void main(String[] args) {
        ContentServer cs = new ContentServer();
        try {
            if (args.length == 0) {
                System.out.println("Usage: java ContentServer <ATOMServer URL> <News path> ");
                System.exit(0);
            }

            String serverURL = args[0];
            File file = new File(args[1]);
            long lastModTime = file.lastModified();
            cs.publish(serverURL, cs.ConvertToXML(file));

            while (true) {
                if (lastModTime != file.lastModified()) {
                    System.out.println("News has been modified. Publishing to ATOMServer\n"
                        + "===============================================");
                    cs.publish(serverURL, cs.ConvertToXML(file));
                    lastModTime = file.lastModified();
                }
                Thread.sleep(2000);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // Generate an Atom feed in xml format by reading from a text file
    public String ConvertToXML(File file) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(file));
        String line = br.readLine();
        if (line == null)
            throw new IOException("Empty file");

        int entryCount = 0;
        String xmlContent = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
                + "<feed xml:lang=\"en-US\" xmlns=\"http://www.w3.org/2005/Atom\">";

        while (line != null) {
            line = line.trim();
            if (line.length() == 0) {
                line = br.readLine();
                continue;
            }
            if (line.matches("^#.*$")) {
               line = br.readLine();
               continue;
            } else if (line.matches("^entry$")) {
                if (entryCount == 0) {
                    xmlContent += "\n<entry>";
                    entryCount++;
                } else {
                    xmlContent += "\n</entry>\n<entry>";
                }
                line = br.readLine();
                continue;
            }

            String[] result = line.split(":");

            if ((result[0].equals("title")) && (result.length == 2)) {
                xmlContent += "\n<title>" + result[1] + "</title>";
            } else if (result[0].equals("subtitle")) {
                xmlContent += "\n<subtitle>" + result[1] + "</subtitle>";
            } else if (result[0].equals("link")) {
                xmlContent += "\n<link href=\"" + result[1] + "\"></link>";
            } else if (result[0].equals("updated")) {
                xmlContent += "\n<updated>";
                for (int i = 1; i < result.length; i++)
                    xmlContent += result[i] + ":";

                xmlContent = xmlContent.substring(0, xmlContent.length() - 1);
                xmlContent += "</updated>";
            } else if (result[0].equals("author")) {
                xmlContent += "\n<author>\n<name>" + result[1] + "</name>\n</author>";
            } else if (result[0].equals("id")) {
                xmlContent += "\n<id>" + result[1] + ":" + result[2] + ":" + result[3] + "</id>";
            } else if (result[0].equals("summary")) {
                xmlContent += "\n<summary>" + result[1] + "</summary>";
            }
            line = br.readLine();
        }
        xmlContent += "\n</entry>\n</feed>";
        return xmlContent;
    }

    // Publish the atom xml format message to AtomServer
    // If the connection lost, the ContentServer will retry to connect to AtomServer.
    public void publish(String url, String xmlContent) throws IOException {
        int response = -1;
        int counter = 1;

        if (!url.matches("^http://.*$"))
            url = "http://" + url;
        url += "/atom.xml";

        while (response != 200) {
            try {
                System.out.println(counter + " time attempts to connect to: " + url);
                URL urlObj = new URL(url);
                HttpURLConnection con = (HttpURLConnection) urlObj.openConnection();
                con.setRequestMethod("PUT");
                con.setRequestProperty("User-Agent", "ATOMClient/1/0");
                con.setRequestProperty("Content-Type", "application/atom+xml");
                con.setRequestProperty("Content-Length", xmlContent.length() + "");
                con.setRequestProperty("lamport-clock", lamportClock + "");
                con.setRequestProperty("Connection", "close");

                con.setDoOutput(true);
                DataOutputStream dos = new DataOutputStream(con.getOutputStream());
                dos.writeBytes(xmlContent);
                dos.flush();
                dos.close();

                response = con.getResponseCode();

                System.out.println("Response Code: " + response + " " + con.getResponseMessage());

                if (response == 200) {
                    int lamportServer = Integer.parseInt(con.getHeaderField("lamport-clock"));
                    lamportClock = Math.max(lamportClock, lamportServer) + 1;
                }

                BufferedReader br = new BufferedReader(new InputStreamReader(con.getInputStream()));
                String line;
                StringBuilder responseMsg = new StringBuilder();

                while ((line = br.readLine()) != null)
                   responseMsg.append(line + "\n");

                System.out.println("Received response: " + responseMsg.toString());
                br.close();
                con.disconnect();

                if (response == 201)
                    response = 200;

            } catch (IOException ie) {
                if (counter == 5) {
                    System.out.println("!!!WARNNING!!!: Maximum retry attempts reached.\n");
                    break;
                }
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException ite) {
                    System.err.println("Received Interrupted Exception");
                }
                counter++;
                System.out.println(ie.getMessage());
            }

        }

    }

}
