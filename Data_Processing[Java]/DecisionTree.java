import java.io.Serializable;
import java.util.ArrayList;
import java.text.*;
import java.lang.Math;

public class DecisionTree implements Serializable {
	DTNode rootDTNode;
	int minSizeDatalist; //minimum number of datapoints that should be present in the dataset so as to initiate a split
	
	//Mention the serialVersionUID explicitly in order to avoid getting errors while deserializing.
	public static final long serialVersionUID = 343L;
	
	public DecisionTree(ArrayList<Datum> datalist , int min) {
		minSizeDatalist = min;
		rootDTNode = (new DTNode()).fillDTNode(datalist);
	}

	class DTNode implements Serializable{
		//Mention the serialVersionUID explicitly in order to avoid getting errors while deserializing.
		public static final long serialVersionUID = 438L;
		boolean leaf;
		int label = -1;      // only defined if node is a leaf
		int attribute; // only defined if node is not a leaf
		double threshold;  // only defined if node is not a leaf



		DTNode left, right; //the left and right child of a particular node. (null if leaf)

		DTNode() {
			leaf = true;
			threshold = Double.MAX_VALUE;
		}



		// this method takes in a datalist (ArrayList of type datum) and a minSizeInClassification (int) and returns
		// the calling DTNode object as the root of a decision tree trained using the datapoints present in the
		// datalist variable
		// Also, KEEP IN MIND that the left and right child of the node correspond to "less than" and "greater than or equal to" threshold
		DTNode fillDTNode(ArrayList<Datum> datalist) {
			
			//DTNode newNode = new DTNode();
			ArrayList<Datum> bestData1 = new ArrayList<Datum>();
			ArrayList<Datum> bestData2 = new ArrayList<Datum>();
			
			double best_avg_entropy = Double.MAX_VALUE;
			int best_attr=-1;
			double best_threshold=-1;
			double curr_avg_entropy = Double.MAX_VALUE;
			
			//YOUR CODE HERE
			if (datalist.size()>=minSizeDatalist) {
			 
				int temp = datalist.get(0).y;
				boolean Lleaf = true;
				
				for (int k=1; k<datalist.size(); k++) {
					//if (!(datalist.get(k).equals(datalist.get(0)))) {
					if(temp!=datalist.get(k).y) {
						Lleaf = false;
						break;
					}
				}
				
				if(Lleaf) {
					this.leaf = true;
					this.label = temp;
					return this;
				}
				
				
					
				//accessing each attribute in [x1, x2]
				for (int i=0; i<2; i++) {
						
					for (Datum item: datalist) {
						
						ArrayList<Datum> data1 = new ArrayList<Datum>();
						ArrayList<Datum> data2 = new ArrayList<Datum>();
						
						double comp_attr=  item.x[i]; 
							
							for (Datum item2: datalist) {
								
								if(item2.x[i] < comp_attr) {
									data1.add(item2);
								} 
								
								else {
									data2.add(item2);
								}
							}	
								curr_avg_entropy = (((double)(data1.size())/((double)datalist.size())*calcEntropy(data1))) + (((double)data2.size())/((double)datalist.size())*calcEntropy(data2));
								
								if(best_avg_entropy > curr_avg_entropy) {
									best_avg_entropy = curr_avg_entropy;
									best_attr = i;
									best_threshold = item.x[i];
									bestData1 = data1;
									bestData2 = data2;
								}
								
							
						}
					}
					
					
						//ArrayList<Datum> data1 = new ArrayList<Datum>();
						//ArrayList<Datum> data2 = new ArrayList<Datum>();
				
						/*for(Datum item3: datalist) {
							if(item3.x[best_attr] < best_threshold) {
								data1.add(item3);
							} else if (item3.x[best_attr] >= best_threshold) {
								data2.add(item3);
							}
						}*/
						
						this.attribute = best_attr;
						this.threshold = best_threshold;
						this.leaf = false;
						
						if((bestData1.size() != 0)) {
						this.left = (new DTNode()).fillDTNode(bestData1);
						}
						if((bestData2.size() != 0)) {
						this.right = (new DTNode()).fillDTNode(bestData2);
						}
			}

			
			
			else {
				this.leaf=true;
				this.label= findMajority(datalist);
			}
				
		
			
		return this;
			
			
	}



		//This is a helper method. Given a datalist, this method returns the label that has the most
		// occurences. In case of a tie it returns the label with the smallest value (numerically) involved in the tie.
		int findMajority(ArrayList<Datum> datalist)
		{
			int l = datalist.get(0).x.length;
			int [] votes = new int[l];

			//loop through the data and count the occurrences of datapoints of each label
			for (Datum data : datalist)
			{
				votes[data.y]+=1;
			}
			int max = -1;
			int max_index = -1;
			//find the label with the max occurrences
			for (int i = 0 ; i < l ;i++)
			{
				if (max<votes[i])
				{
					max = votes[i];
					max_index = i;
				}
			}
			return max_index;
		}




		// This method takes in a datapoint (excluding the label) in the form of an array of type double (Datum.x) and
		// returns its corresponding label, as determined by the decision tree
		int classifyAtNode(double[] xQuery) {
			//YOUR CODE HERE
			if(this.leaf==true) {
				return this.label;
			} 
			
			else {
				if(xQuery[this.attribute]<this.threshold) {
					return this.left.classifyAtNode(xQuery);
				} else {
					return this.right.classifyAtNode(xQuery);
				}
			}
		}


		//given another DTNode object, this method checks if the tree rooted at the calling DTNode is equal to the tree rooted
		//at DTNode object passed as the parameter
		public boolean equals(Object dt2)
		{

			//YOUR CODE HERE
			//*must cast the object dt2 to a node first!
			//for internal node, the nodes are equal if the thresholds and attributes are same
			//for leaf node, the labels should be same. 
			//but before doing that, you have to check if the node is a leaf node first and has the label. 
			//for the traversal, you have to use the equals function to check if the left and right nodes exists first 
			//of the two trees are the same (ie. recursion).
			
			//casting the dt2 object into nodes
			DTNode node2 = (DTNode) dt2;
			

			if (this.leaf==true && node2.leaf==true) {
				if(this.label==node2.label) {
					return true;
				} else {
					return false;
				}
			} 
			
			//**figure out when it's an internal node and check for the left and right child nodes. 
		
			
			else {
			
				//check if it's not a leaf b/c otherwise they won't have left or right node. 
					
					if(!(this.threshold==node2.threshold || this.attribute==node2.attribute)) {
						return false;
					} 
					
					boolean thisNodeLeft = (this.left!=null);
					boolean thisNodeRight = (this.right!=null);
					boolean node2Left = (node2.left!=null);
					boolean node2Right = (node2.right!=null);
					
					//checking for inconsistency. 
					if (thisNodeLeft!=node2Left || thisNodeRight!=node2Right) {
						return false;
					}
					
					if(thisNodeLeft==true && node2Left==true && thisNodeRight==false && node2Right==false) {
						return (this.left.equals(node2.left));
					}
					
					if (thisNodeRight==true && node2Right==true  && thisNodeLeft==false && node2Left==false) {
						return (this.right.equals(node2.right));
					}
					
					if (thisNodeRight==true && thisNodeLeft==true && node2Left==true && node2Right==true) {
						if ( (!this.left.equals(node2.left)) || (!this.right.equals(node2.right))) {
							return false;
						}
						return true;
					}
				}
			
		
			return false;
		}
	}



	//Given a dataset, this retuns the entropy of the dataset
	double calcEntropy(ArrayList<Datum> datalist)
	{
		double entropy = 0;
		double px = 0;
		float [] counter= new float[2];
		if (datalist.size()==0)
			return 0;
		double num0 = 0.00000001,num1 = 0.000000001;

		//calculates the number of points belonging to each of the labels
		for (Datum d : datalist)
		{
			counter[d.y]+=1;
		}
		//calculates the entropy using the formula specified in the document
		for (int i = 0 ; i< counter.length ; i++)
		{
			if (counter[i]>0)
			{
				px = counter[i]/datalist.size();
				entropy -= (px*Math.log(px)/Math.log(2));
			}
		}

		return entropy;
	}


	// given a datapoint (without the label) calls the DTNode.classifyAtNode() on the rootnode of the calling DecisionTree object
	int classify(double[] xQuery ) {
		DTNode node = this.rootDTNode;
		return node.classifyAtNode( xQuery );
	}

    // Checks the performance of a DecisionTree on a dataset
    //  This method is provided in case you would like to compare your
    //results with the reference values provided in the PDF in the Data
    //section of the PDF

    String checkPerformance( ArrayList<Datum> datalist)
	{
		DecimalFormat df = new DecimalFormat("0.000");
		float total = datalist.size();
		float count = 0;

		for (int s = 0 ; s < datalist.size() ; s++) {
			double[] x = datalist.get(s).x;
			int result = datalist.get(s).y;
			if (classify(x) != result) {
				count = count + 1;
			}
		}

		return df.format((count/total));
	}


	//Given two DecisionTree objects, this method checks if both the trees are equal by
	//calling onto the DTNode.equals() method
	public static boolean equals(DecisionTree dt1,  DecisionTree dt2)
	{
		boolean flag = true;
		flag = dt1.rootDTNode.equals(dt2.rootDTNode);
		return flag;
	}

}
