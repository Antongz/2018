/* a1691807, Yi-Ting, Hsieh */
import java.util.*;

/*****************************************************************************
*  FeedEntry
*  1. Stores the FeedEntry properties.
*  2. Provide functions to get access to the content of the FeedEntry.
*****************************************************************************/
public class FeedEntry {
    private String title, link, id, summary;
    private Date updatedate;

    public FeedEntry(String title, String link, String id, String date, String summary) {
        this.title = title;
        this.link = link;
        this.id = id;
        Calendar cl = javax.xml.bind.DatatypeConverter.parseDateTime(date);
        updatedate = cl.getTime();
        this.summary = summary;
    }

    public void setTitle(String title) { this.title = title; }

    public String getTitle() { return this.title; }

    public void setEntryID(String id) { this.id = id; }

    public String getEntryId() { return this.id; }

    public void setLink(String link) { this.link = link; }

    public String getLink() { return this.link; }

    public void setDate(String date) {
        Calendar cl = javax.xml.bind.DatatypeConverter.parseDateTime(date);
        this.updatedate = cl.getTime();
    }

    public Date getDate() { return this.updatedate; }

    public void setSummary(String summary) { this.summary = summary; }

    public String getSummary() { return this.summary; }

}
