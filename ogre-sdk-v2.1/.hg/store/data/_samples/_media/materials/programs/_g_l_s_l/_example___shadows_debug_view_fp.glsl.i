         �   �      ���������-��� xo�%4�)��YUI�            x�M�A�@��ί𲂈e7�F]A��N���ĺ+F��;Dsz|��1���'���H�Gx��[�^�.I�|���~r�r��*���97�"��PZ8S�[CÖ���3w�#Nz|X��#� xf3਍S� �~���+
-!zRb��_0+�7����ۤ<L     �     q   p     �    �����O�E$�1h3Y���G�a��            u
uniform sampler2D rt;

void main()
{
    gl_FragColor = vec4( texture2D( rt, gl_TexCoord[0].xy ).xxx, 1.0 );
}
