/* a1691807 Yi-Ting, Hsieh */

import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;

public final class RandomArticleProducer {

	private static int minSentencesPerFile = 30;
	private static int sentencesRange = 50;
	private static int minWordsPerSentence = 40;
	private static int wordsRange = 70;
	private static int minCharsPerWord = 1;
	private static int charsRange = 16;
	private static int letterAmonut = 26;
	private static int numberAmonut = 10;
	private static char[] wordPool = new char[letterAmonut * 2 + numberAmonut];
	private static String[] symbolPool = { ".\r\n\r\n", ". ", ". ", ", ", ",",
	", ", "? ", "! ", ": ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "
	, " ", " ", " ", " ", " ", " ", " ", " " };
	private static int lengthOfWordPool = wordPool.length;
	private static int lengthOfSymbolPool = symbolPool.length;

	private static RandomArticleProducer rap = null;

	private RandomArticleProducer() {

	}

	static {
		rap = new RandomArticleProducer();
		rap.setWordPool();
	}

	private void setWordPool() {
		int i = 0;
		while (i < lengthOfWordPool) {
			if (i < letterAmonut) {
				wordPool[i] = (char) ('A' + i);
			} else if (i < letterAmonut * 2) {
				wordPool[i] = (char) ('a' + i - letterAmonut);
			} else if (i < letterAmonut * 2 + numberAmonut) {
				wordPool[i] = (char) ('0' + i - letterAmonut * 2);
			}
			i++;
		}
	}

	public static void genFile(String fileName) {
		try (Writer out = new FileWriter(fileName)) {
			String content = rap.genContent();
			out.write(content);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private String genContent() {
		StringBuilder sBuilder = new StringBuilder();
		rap.feedContent(sBuilder);
		return sBuilder.toString();
	}

	private void feedContent(StringBuilder sBuilder) {
		int numberOfSentences = minSentencesPerFile + (int) (Math.random() *
		sentencesRange);

		for (int i = 0; i < numberOfSentences + 1; i++)
			rap.feedSentence(sBuilder);
	}

	private void feedSentence(StringBuilder sBuilder) {
		int numberOfWords = minWordsPerSentence + (int) (Math.random() *
		wordsRange);

		for (int i = 0; i < numberOfWords + 1; i++)
			rap.feedWord(sBuilder);
	}

	private void feedWord(StringBuilder sBuilder) {
		int numberOfChars = minCharsPerWord + (int) (Math.random() *
		charsRange);

		for (int i = 0; i < numberOfChars + 1; i++)
			sBuilder.append(wordPool[(int) (Math.random() * lengthOfWordPool)]);
		sBuilder.append(symbolPool[(int) (Math.random() * lengthOfSymbolPool)]);
	}
}
