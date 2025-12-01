LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE std.textio.all;
USE ieee.std_logic_textio.all;
use IEEE.NUMERIC_STD.ALL;
 
 
ENTITY MD5_tb IS
END MD5_tb;
 
ARCHITECTURE behavior OF MD5_tb IS 
 
    COMPONENT MD5
    PORT(
         data_in : IN  std_logic_vector(31 downto 0);
         data_out : OUT  std_logic_vector(31 downto 0);
         done : OUT  std_logic;
         start:       in  STD_LOGIC;
         clk : IN  std_logic;
         reset : IN  std_logic
        );
    END COMPONENT;
    

   signal data_in : std_logic_vector(31 downto 0) := (others => '0');
   signal start : std_logic := '0';
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';

   signal data_out : std_logic_vector(31 downto 0);
   signal done : std_logic;

   constant clk_period : time := 5 ns;

   type msg_t is array(0 to 15) of std_logic_vector(31 downto 0);

   signal message : msg_t := (X"24cda8da", X"aed64a2e", X"312f765a", X"b90c9791",
                              X"fd32d9d0", X"615206cb", X"5b0e5045", X"dbb5f6af",
                              X"f4310a8e", X"58468968", X"c3b8c9aa", X"24db1a8d",
                              X"aec70000", X"00000000", X"00000000", X"00000000");

    signal message_length : std_logic_vector(31 downto 0) := (others => '0');
    signal hash : std_logic_vector(0 to 127) := (others => '0');
BEGIN
 
   uut: MD5 PORT MAP (
          data_in => data_in,
          data_out => data_out,
          done => done,
          start => start,
          clk => clk,
          reset => reset
        );

   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;

   stim_proc: process
   begin
       reset <= '1';
       wait for clk_period;
       reset <= '0';
       message_length <= std_logic_vector(to_unsigned(400, message_length'length));
       start <= '1';
       wait for 0.4*clk_period;

       data_in <= message_length;
       wait for 2*clk_period;
       start <= '0';

       for i in 0 to 15 loop
           data_in <= message(i);
           wait for clk_period;
       end loop;

       wait for 140*clk_period;
       start <= '1';
       wait for clk_period;
       start <= '0';
       wait for clk_period;

       for i in 0 to 3 loop
           hash(32*i to 32*i+31) <= data_out;
           wait for clk_period;
       end loop;

       wait for 10*clk_period;

      assert false severity failure;
   end process;

END;