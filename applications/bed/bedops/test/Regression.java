// Author
//  Shane Neph : 2006

import org.w3c.dom.*;
import javax.xml.parsers.*;
import java.io.*;
import java.util.*;

//==========
// TagNames
//==========
class TagNames {
  final static String TEST   = "TEST";
  final static String INPUT  = "INPUT";
  final static String ANSWER = "ANSWER";
  final static String CALL   = "CALL";
  final static String OUTPUT = "OUTPUT";

  final static String ATT_NAME  = "name";
  final static String ATT_ORDER = "order";
  final static String ATT_CHROM = "chromosome";
}

//============
// Regression
//============
class Regression {
  Regression(File f) throws Exception {
    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    DocumentBuilder builder = factory.newDocumentBuilder();
    Document doc = builder.parse(f);
    root_ = doc.getDocumentElement();
    NodeList c = root_.getChildNodes();
    tests_ = new java.util.ArrayList();
    for ( int i = 0; i < root_.getChildNodes().getLength(); ++i ) {
      if ( c.item(i) instanceof Element )
        tests_.add(tests_.size(), new Test((Element)(c.item(i))));
    } // for
    Collections.sort(tests_);
  }

  Test NextTest() {
    return((Test)(tests_.get(next_++)));
  }

  int Size() {
    return(tests_.size());
  }

  //========
  // Main()
  //========
  static public void main(String[] str) {
    try {
      if ( str.length < 1 )
        throw(new Exception("Expect a .xml test plan file input"));
      Regression tp = new Regression(new File(str[0]));
      if ( tp.Size() == 0 ) 
        throw(new Exception("Unable to find tests in TestPlan.xml"));
      boolean status = true;
      for ( int i = 0; i < tp.Size(); ++i ) {
        Test t = tp.NextTest();
        System.out.println(t.Name());
        try {
          boolean result = t.Perform();
          status = result && status;
          if ( result )
            System.out.println("  PASSED");
          else
            System.out.println("  **FAILED**");
        } catch(SecurityException e) {
          throw(new Exception(e.getMessage() + "\nUnable to make system call due to security manager"));
        }
      }
      if ( status )
        System.out.println("\n\n-- PASSED OVERALL! --\n\n");
      else
        System.out.println("\n\n-- FAILURE(S) DETECTED! --\n\n");
    } catch(Exception e) {
      System.err.println("Error occured:\n" + e.getMessage());
    }
  }

private Element root_;
private int next_ = 0;
private java.util.ArrayList tests_;
}

//======
// Test
//======
class Test implements Comparable {
  static final String TAB = "\t";

  Test(Element test) throws Exception {
    order_ = Integer.valueOf(test.getAttribute(TagNames.ATT_ORDER)).intValue();
    chr_ = test.getAttribute(TagNames.ATT_CHROM);
    output_ = null;
    boolean foundAnswer = false;
    NodeList children = test.getChildNodes();
    for ( int i = 0; i < children.getLength(); ++i ) {
      Node child = children.item(i);
      if ( child instanceof Element ) {
        Element next = (Element)child;
        if ( next.getTagName().equals(TagNames.CALL) ) {
          Text tn = (Text)(next.getFirstChild());
          call_ += tn.getData().trim();
        }
        else if ( next.getTagName().equals(TagNames.OUTPUT) )
          output_ = next.getAttribute(TagNames.ATT_NAME);
        else if ( next.getTagName().equals(TagNames.INPUT) ) {
          inputFiles_ += " " + next.getAttribute(TagNames.ATT_NAME);
          FileWriter out = new FileWriter(new File(next.getAttribute(TagNames.ATT_NAME)));
          Text textNode = (Text)(next.getFirstChild());
          out.write(updateString(textNode.getData().trim()));
          out.close();
        }
        else if ( next.getTagName().equals(TagNames.ANSWER) ) {
          if ( foundAnswer )
            throw(new Exception("Cannot have multiple answers - test# " + String.valueOf(order_)));
          foundAnswer = true;
          Text textNode = (Text)(next.getFirstChild());
          if ( textNode != null )
            answer_ = updateString(textNode.getData().trim());
          else
            answer_ = "";
        }
      }
    } // for

    if ( inputFiles_.length() > 0 )
      call_ += inputFiles_;
    if ( output_ == null )
      throw(new Exception("No output file specified for test# " + String.valueOf(order_)));
  }

  public int compareTo(Object o) {
    Test t = (Test)(o);
    if ( this.order_ < t.order_ )
      return(-1);
    else if ( this.order_ > t.order_ )
      return(1);
    return(0);
  }

  String Name() {
    return("Test" + String.valueOf(order_) + ": " + call_ + " > " + output_);
  }

  boolean Perform() throws Exception {
    Runtime rt = Runtime.getRuntime();
    Process p = rt.exec(call_);
    int check = p.waitFor();
    if ( check != 0 || p.exitValue() != 0 )
      throw(new Exception("Unable to start bedops"));
    BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));
    BufferedReader stdErr = new BufferedReader(new InputStreamReader(p.getErrorStream()));
    String subErr = stdErr.readLine();
    if ( subErr != null ) {
      String all;
      while ( (all = stdErr.readLine()) != null )
        subErr += "\n" + all;
      throw(new Exception(subErr));
    }
    String subCout = "";
    String tmp;
    while ( (tmp = stdInput.readLine()) != null ) {
      if ( tmp.length() > 0 )
        subCout += tmp.trim() + "\n";
    } // while

    File f = new File(output_);
    BufferedWriter bw = new BufferedWriter(new FileWriter(f));
    bw.write(subCout);
    bw.close();
    return(subCout.equals(answer_));
  }

  private String updateString(String s) {
    for ( int i = 0; i < s.length(); ++i ) {
      if ( s.charAt(i) == ' ' ) {
        s = s.substring(0, i) + s.substring(i+1);
        --i;
      }
    } // for

    String toRtn = "";
    StringTokenizer tk = new StringTokenizer(s, "\n");
    int size = tk.countTokens();
    for ( int i = 0; i < size; ++i ) {
      if ( chr_ != null && chr_.length() > 0 )
        toRtn += chr_ + TAB;
      toRtn += tk.nextToken() + "\n";
    }
    return(toRtn);
  }

private int order_         = -1;
private String call_       = "../../bin/bedops-typical --ec ";
private String inputFiles_ = "";
private String answer_     = "";
private String output_     = "";
private String chr_        = "";
}
