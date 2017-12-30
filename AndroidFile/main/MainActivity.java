package sampleactivities.metropolia.org.senddata;

import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.Scanner;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.mime.HttpMultipartMode;
import org.apache.http.entity.mime.MultipartEntity;
import org.apache.http.entity.mime.MultipartEntityBuilder;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.renderscript.RenderScript;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

    String name;
    Button button;
    Button button1;
    Button button2;
    private TextView alertTextView;
    public boolean warning = false;

    // define url
    public static String URL = "http://users.metropolia.fi/~hieuv/readAndroidData.php";

    private ProgressDialog Dialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        button1 = (Button) findViewById(R.id.button1);
        button2 = (Button) findViewById(R.id.button2);
        button = (Button)findViewById(R.id.button);

        new DisplayStatus().execute("");
        new DisplayTime().execute("");
        new DisplayTemp().execute("");
        new DisplayHum().execute("");

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = getIntent();
                finish();
                startActivity(intent);
            }
        });


        button1.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                if(warning == true){
                    new button2task().execute();
                    Intent intent = getIntent();
                    finish();
                    startActivity(intent);
                }else{
                    // TODO Auto-generated method stub
                    new button1task().execute();
                    Intent intent = getIntent();
                    finish();
                    startActivity(intent);
                }

            }
        });

        button2.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                // TODO Auto-generated method stub
                new button2task().execute();
                Intent intent = getIntent();
                finish();
                startActivity(intent);
            }
        });

        Thread t = new Thread() {
            @Override
            public void run() {
                try {
                    while (!isInterrupted()) {
                        Thread.sleep(30000);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                // update TextView here!
                                Intent intent = getIntent();
                                finish();
                                startActivity(intent);
                            }
                        });
                    }
                } catch (InterruptedException e) {
                }
            }
        };
        t.start();

        Dialog = new ProgressDialog(MainActivity.this);

        alertTextView = (TextView) findViewById(R.id.AlertTextView);

    }



    public void Alert(){
        //if(warning){
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);

        builder.setCancelable(true);
        builder.setTitle("High Temperature Alert Dialog");
        builder.setMessage("Fire Risk Alert !");

        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                dialogInterface.cancel();
            }
        });

        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                alertTextView.setVisibility(View.VISIBLE);
            }
        });
        builder.show();
        //}
    }

    private class button1task extends AsyncTask<String, Void, Void> {

        private final HttpClient Client = new DefaultHttpClient();
        private String Content;
        private String Error = null;
        private final String TAG = null;

        @Override
        protected void onPreExecute() {
            Dialog.setMessage("Wait..");
            Dialog.show();
            Log.e(TAG, "------------------------------------- Here");
        }

        protected Void doInBackground(String... urls) {

            try {
                HttpPost httppost = new HttpPost(URL);

                // Field
                JSONObject jObject = new JSONObject();
                jObject.put("name", "1");

                MultipartEntityBuilder se = MultipartEntityBuilder.create();
                        se.setMode(HttpMultipartMode.BROWSER_COMPATIBLE);
                se.addPart("request", new StringBody(jObject.toString(), ContentType.TEXT_PLAIN));
                HttpEntity entity = se.build();

                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                entity.writeTo(baos);
                Log.d("seMain", baos.toString());
                httppost.setEntity(entity);

                HttpResponse resp = Client.execute(httppost);

                Content = EntityUtils.toString(resp.getEntity());

                Log.e("Response", "--------------------------------" + Content);

            } catch (JSONException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (UnsupportedEncodingException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (ClientProtocolException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            // TODO Auto-generated method stub
            Dialog.dismiss();
            // Log.e("ERROR", "--------------------------------" + Content);
            Toast.makeText(getBaseContext(), Content, Toast.LENGTH_LONG).show();
        }

    }

    private class button2task extends AsyncTask<String, Void, Void> {

        private final HttpClient Client = new DefaultHttpClient();
        private String Content;
        private String Error = null;
        private final String TAG = null;

        @Override
        protected void onPreExecute() {
            Dialog.setMessage("Wait..");
            Dialog.show();
            Log.e(TAG, "------------------------------------- Here");
        }

        protected Void doInBackground(String... urls) {

            try {
                HttpPost httppost = new HttpPost(URL);

                // Field
                JSONObject jObject = new JSONObject();
                jObject.put("name", "0");

                MultipartEntityBuilder se = MultipartEntityBuilder.create();
                se.setMode(HttpMultipartMode.BROWSER_COMPATIBLE);
                se.addPart("request", new StringBody(jObject.toString(), ContentType.TEXT_PLAIN));
                HttpEntity entity = se.build();

                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                entity.writeTo(baos);
                Log.d("seMain", baos.toString());
                httppost.setEntity(entity);

                HttpResponse resp = Client.execute(httppost);

                Content = EntityUtils.toString(resp.getEntity());

                Log.e("Response", "--------------------------------" + Content);

            } catch (JSONException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (UnsupportedEncodingException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (ClientProtocolException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            // TODO Auto-generated method stub
            Dialog.dismiss();
            // Log.e("ERROR", "--------------------------------" + Content);
            Toast.makeText(getBaseContext(), Content, Toast.LENGTH_LONG).show();
        }

    }

    public class DisplayStatus extends AsyncTask<String, Void, ArrayList<String>> {
        @Override
        protected ArrayList<String> doInBackground(String... params) {
            ArrayList<String> respList = new ArrayList<>();

            String stringHTTP = deleteBlanks("http://users.metropolia.fi/~hieuv/convertAndroidData.php");

            String contentAsString = "";
            HttpURLConnection urlConnection = null;
            try {
                // create connection
                java.net.URL urlToRequest = new URL(stringHTTP);
                urlConnection = (HttpURLConnection) urlToRequest.openConnection();
                urlConnection.setReadTimeout(10000); //miliseconds
                urlConnection.setConnectTimeout(15000); //miliseconds
                // handle issues
                int statusCode = urlConnection.getResponseCode();
                Log.d("MainActivity", "The response is: " + statusCode);
                if (statusCode == HttpURLConnection.HTTP_UNAUTHORIZED) {
                    // handle unauthorized (if service requires user login)
                } else if (statusCode != HttpURLConnection.HTTP_OK) {
                    // handle any other errors, like 404, 500,..
                }
                // create JSON object from content
                InputStream in = new BufferedInputStream(urlConnection.getInputStream());
                contentAsString = new Scanner(in).useDelimiter("\\A").next();

                System.out.println(contentAsString);

                JSONArray entryArray = new JSONArray(contentAsString);
                String resp;

                for (int i = 0; i < 1; i++) {
                    resp = entryArray.getJSONObject(i).getString("name");
                    respList.add(resp);
                }

            }
            catch (Exception e) {
                e.printStackTrace();
            }
            return respList;
        }

        protected void onPostExecute(ArrayList<String> list)
        {

            TextView t = findViewById(R.id.result5);
            String result5 = "Status:\n";
            for (String s: list) {
                Log.d("Main", s);
                int y = Integer.parseInt(s);
                if(y == 0){
                    result5 += "Off\n";
                }else{
                    result5 += "On\n";
                }

            }
            t.setText(result5);
        }
    }

    public String deleteBlanks(String stringHTTP)
    {
        //Replace the blanks in the URL
        return stringHTTP.replace(" ", "%20");
    }

    @Override
    protected void onPause(){
        super.onPause();
        Dialog.dismiss();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    public class DisplayTime extends AsyncTask<String, Void, ArrayList<String>> {
        @Override
        protected ArrayList<String> doInBackground(String... params) {
            ArrayList<String> respList = new ArrayList<>();

            String stringHTTP = deleteBlanks("http://users.metropolia.fi/~hieuv/ConvertPHPtoJSON.php");

            String contentAsString = "";
            HttpURLConnection urlConnection = null;
            try {
                // create connection
                URL urlToRequest = new URL(stringHTTP);
                urlConnection = (HttpURLConnection) urlToRequest.openConnection();
                urlConnection.setReadTimeout(10000); //miliseconds
                urlConnection.setConnectTimeout(15000); //miliseconds
                // handle issues
                int statusCode = urlConnection.getResponseCode();
                Log.d("MainActivity", "The response is: " + statusCode);
                if (statusCode == HttpURLConnection.HTTP_UNAUTHORIZED) {
                    // handle unauthorized (if service requires user login)
                } else if (statusCode != HttpURLConnection.HTTP_OK) {
                    // handle any other errors, like 404, 500,..
                }
                // create JSON object from content
                InputStream in = new BufferedInputStream(urlConnection.getInputStream());
                contentAsString = new Scanner(in).useDelimiter("\\A").next();

                System.out.println(contentAsString);

                JSONArray entryArray = new JSONArray(contentAsString);
                String resp;

                for (int i = 0; (i < entryArray.length() -1) && i < 10; i++) {
                    resp = entryArray.getJSONObject(i).getString("timeStamp");
                    respList.add(resp);
                }

            }
            catch (Exception e) {
                e.printStackTrace();
            }
            return respList;
        }

        protected void onPostExecute(ArrayList<String> list)
        {
            TextView t = findViewById(R.id.result4);
            String result4 = "Time\n";
            for (String s: list) {
                Log.d("Main", s);
                result4 += s + "\n";
            }
            t.setText(result4);
        }
    }

    public class DisplayTemp extends AsyncTask<String, Void, ArrayList<String>> {
        @Override
        protected ArrayList<String> doInBackground(String... params) {
            ArrayList<String> respList = new ArrayList<>();

            String stringHTTP = deleteBlanks("http://users.metropolia.fi/~hieuv/ConvertPHPtoJSON.php");

            String contentAsString = "";
            HttpURLConnection urlConnection = null;
            try {
                // create connection
                URL urlToRequest = new URL(stringHTTP);
                urlConnection = (HttpURLConnection) urlToRequest.openConnection();
                urlConnection.setReadTimeout(10000); //miliseconds
                urlConnection.setConnectTimeout(15000); //miliseconds
                // handle issues
                int statusCode = urlConnection.getResponseCode();
                Log.d("MainActivity", "The response is: " + statusCode);
                if (statusCode == HttpURLConnection.HTTP_UNAUTHORIZED) {
                    // handle unauthorized (if service requires user login)
                } else if (statusCode != HttpURLConnection.HTTP_OK) {
                    // handle any other errors, like 404, 500,..
                }
                // create JSON object from content
                InputStream in = new BufferedInputStream(urlConnection.getInputStream());
                contentAsString = new Scanner(in).useDelimiter("\\A").next();

                System.out.println(contentAsString);

                JSONArray entryArray = new JSONArray(contentAsString);
                String resp;

                for (int i = 0; (i < entryArray.length() -1) && i < 10; i++) {
                    resp = entryArray.getJSONObject(i).getString("temperature");
                    respList.add(resp);
                    if(i == 0){
                        int temp = Integer.parseInt(resp);
                        if(temp > 80){
                            warning = true;
                        }
                    }
                }

            }
            catch (Exception e) {
                e.printStackTrace();
            }
            return respList;
        }
        protected void onPostExecute(ArrayList<String> list)
        {
            TextView t = findViewById(R.id.result2);
            String result2 = "Temperature\n";
            for (String s: list) {
                Log.d("Main", s);
                result2 += s + "\n";
            }
            t.setText(result2);
            if(warning){Alert(); new button2task().execute();}
        }
    }

    public class DisplayHum extends AsyncTask<String, Void, ArrayList<String>> {
        @Override
        protected ArrayList<String> doInBackground(String... params) {
            ArrayList<String> respList = new ArrayList<>();

            String stringHTTP = deleteBlanks("http://users.metropolia.fi/~hieuv/ConvertPHPtoJSON.php");

            String contentAsString = "";
            HttpURLConnection urlConnection = null;
            try {
                // create connection
                URL urlToRequest = new URL(stringHTTP);
                urlConnection = (HttpURLConnection) urlToRequest.openConnection();
                urlConnection.setReadTimeout(10000); //miliseconds
                urlConnection.setConnectTimeout(15000); //miliseconds
                // handle issues
                int statusCode = urlConnection.getResponseCode();
                Log.d("MainActivity", "The response is: " + statusCode);
                if (statusCode == HttpURLConnection.HTTP_UNAUTHORIZED) {
                    // handle unauthorized (if service requires user login)
                } else if (statusCode != HttpURLConnection.HTTP_OK) {
                    // handle any other errors, like 404, 500,..
                }
                // create JSON object from content
                InputStream in = new BufferedInputStream(urlConnection.getInputStream());
                contentAsString = new Scanner(in).useDelimiter("\\A").next();

                System.out.println(contentAsString);

                JSONArray entryArray = new JSONArray(contentAsString);
                String resp;

                for (int i = 0; (i < entryArray.length() -1) && i < 10; i++) {
                    resp = entryArray.getJSONObject(i).getString("humidity");
                    respList.add(resp);
                }
            }
            catch (Exception e) {
                e.printStackTrace();
            }
            return respList;
        }

        protected void onPostExecute(ArrayList<String> list)
        {
            TextView t = findViewById(R.id.result3);
            String result3 = "Humidity\n";
            for (String s: list) {
                Log.d("Main", s);
                result3 += s + "\n";
            }
            t.setText(result3);
        }
    }





}