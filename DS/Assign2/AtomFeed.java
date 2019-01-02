/* a1691807, Yi-Ting, Hsieh */
import java.util.ArrayList;
import java.util.*;

/*****************************************************************************
*  AtomFeed
*  1. Stores the Atom feed properties.
*  2. Provide functions to get access to the content of the feed.
*****************************************************************************/
public class AtomFeed {
    private String title, subtitle, link, author, id;
    private Date updatedate;
    private final List<FeedEntry> entries;


    public AtomFeed() {
        entries = new ArrayList<FeedEntry>();
    }

    public AtomFeed(String title, String subtitle, String link, String date, String author, String id) {
        this.title = title;
        this.subtitle = subtitle;
        this.link = link;
        Calendar cl = javax.xml.bind.DatatypeConverter.parseDateTime(date);
        updatedate = cl.getTime();
        this.author = author;
        this.id = id;
        this.entries = new ArrayList<FeedEntry>();

    }

    public void setTitle(String title) { this.title = title; }

    public String getTitle() { return this.title; }

    public void setSubtitle(String subtitle) { this.subtitle = subtitle; }

    public String getSubtitle() { return this.subtitle; }

    public void setFeedID(String id) { this.id = id; }

    public String getFeedId() { return this.id; }

    public void setLink(String link) { this.link = link; }

    public String getLink() { return this.link; }

    public void setDate(String date) {
        Calendar cl = javax.xml.bind.DatatypeConverter.parseDateTime(date);
        this.updatedate = cl.getTime();
    }

    public Date getDate() { return this.updatedate; }

    public void setAuthor(String author) { this.author = author; }

    public String getAuthor() { return this.author; }

    public void addEntry(FeedEntry fe) { entries.add(fe); }

    public List getEntries() { return entries; }

}
